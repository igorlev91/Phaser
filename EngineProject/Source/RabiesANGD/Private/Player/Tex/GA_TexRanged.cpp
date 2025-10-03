// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexRanged.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"


#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Player/Dot/RDot_RangedRevUpCooldown.h"
#include "Player/Dot/RDot_RangedAttack_Cooldown.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_TexRanged::UGA_TexRanged()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_TexRanged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	cooldownHandle = Handle;
	actorInfo = ActorInfo;
	activationInfo = ActivationInfo;

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	if (Player)
	{
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}

	if (Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUltimateAttackAimingTag()))
	{
		GetWorld()->GetTimerManager().SetTimer(UltimateTimerHandle, this, &UGA_TexRanged::Fire, 1.0f, false);

		FTimerHandle noiseTiemr;
		GetWorld()->GetTimerManager().SetTimer(noiseTiemr, this, &UGA_TexRanged::UltNoise, 0.4f, false);
		Player->ServerPlayAnimMontage(UltimateMontage);
	}
	else
	{
		UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootingMontage);
		playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_TexRanged::K2_EndAbility);
		playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_TexRanged::K2_EndAbility);
		playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_TexRanged::K2_EndAbility);
		playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_TexRanged::K2_EndAbility);
		playTargettingMontageTask->ReadyForActivation();


		if (K2_HasAuthority())
		{
			if (AbilitySoundEfx.Num() > 0)
			{
				Player->PlayVoiceLine(AbilitySoundEfx, 40);
			}
		}

		TriggerAudioCue();
		Fire();
	}
}

void UGA_TexRanged::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}

void UGA_TexRanged::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	if (K2_HasAuthority())
	{
		bool bFound = false;
		float timer = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetCritComboTimerAttribute(), bFound);

		if (timer <= 0)
		{
			FGameplayEffectSpecHandle EffectSpec45 = MakeOutgoingGameplayEffectSpec(CritShotReset, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec45);
		}

		FGameplayEffectSpecHandle EffectSpec2121 = MakeOutgoingGameplayEffectSpec((bIsCrit) ? AddCritShot : CritShotReset, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec2121);

		if (timer <= 0 && bIsCrit)
		{
			FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AddTimeToCritTimer, 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
			FGameplayEffectSpec* spec = specHandle.Data.Get();
			if (spec)
			{
				Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
			}
		}

		if (hitActor == nullptr) return;
		if (hitActor != Player)
		{
			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState)
			{
				gameState->Multicast_ShootTexUltimate(ShootingParticle, Player, startPos, endPos, endPos);
				gameState->Multicast_RequestSpawnVFX(BulletCasing, Player->GetMesh()->GetSocketLocation("BulletCasing"), -Player->GetActorRightVector(), 102);
			}

			AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitActor);
			if (hitEnemy == nullptr)
				return;

			if (gameState)
			{
				int gibletClount = (bIsCrit) ? hitEnemy->GibletCount + 2 : hitEnemy->GibletCount -1 ;
				gameState->Multicast_RobotGiblets(endPos, -Player->GetActorForwardVector(), gibletClount);
			}

			//FVector hitPointVector = hitEnemy->GetMesh()->GetSocketLocation(hitEnemy->WeakpointSocketName);
			//UE_LOG(LogTemp, Warning, TEXT("Value is: %f"), FVector::Dist(endPos, hitPointVector));

			if (bIsCrit)
			{
				Player->GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive); // Texs invis
			}

			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec((bIsCrit) ? CritRangedDamage : RangedDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

			if (ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(hitActor))
				Player->HitRangedAttack(hitCharacter); // make sure to do it afterwards so you can check health

			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_TexRanged::UltNoise()
{
	if (K2_HasAuthority())
	{
		if (AbilityUltimateSoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilityUltimateSoundEfx, 100);
		}
	}
}

void UGA_TexRanged::Fire()
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetRangedAttackStrengthTag(), FGameplayEventData());
	if (K2_HasAuthority())
	{
		if (Player)
		{
			Player->Hitscan(12000, Player->GetPlayerBaseState());

			K2_EndAbility();
		}
	}
}
