// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_RangedGattlingAttack.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "Player/RPlayerController.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Framework/EOSActionGameState.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Enemy/REnemyBase.h"

#include "Player/Dot/RDot_RangedRevUpCooldown.h"
#include "Player/Dot/RDot_RangedAttack_Cooldown.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_RangedGattlingAttack::UGA_RangedGattlingAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_RangedGattlingAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	UAbilityTask_WaitGameplayEvent* WaitTargetAquiredEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitTargetAquiredEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::Fire);
	WaitTargetAquiredEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAttackTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::StopAttacking);
	WaitStopEvent->ReadyForActivation();

	if (Player)
	{
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	SetupWaitInputTask();
}

void UGA_RangedGattlingAttack::Fire(FGameplayEventData Payload)
{
	if (CheckCooldown(cooldownHandle, actorInfo))
	{
		if (K2_HasAuthority())
		{
			if (Player)
			{
				FGameplayEffectContextHandle EffectContext = Player->GetAbilitySystemComponent()->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				CooldownCalculationClass = ShootingCooldownClass;

				FGameplayEffectSpecHandle cooldownSpecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(ShootingCooldownGameplayEffect, GetAbilityLevel(), EffectContext);
				if (cooldownHandle.IsValid())
				{
					ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, cooldownSpecHandle);
				}

				//ApplyCooldown(cooldownHandle, actorInfo, activationInfo);
				Player->Hitscan(12000, Player->GetPlayerBaseState());

				StartDurationAudioEffect();
			}
		}
	}
}

void UGA_RangedGattlingAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		StopDurationAudioEffect();
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}

void UGA_RangedGattlingAttack::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{

	if (K2_HasAuthority())
	{
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
				gameState->Multicast_RobotGiblets(endPos, -Player->GetActorForwardVector(), hitEnemy->GibletCount);
			}

			hitEnemy->DamagedByPlayer = Player;

			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RangedGattlingDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

			if (ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(hitActor))
			{
				Player->HitRangedAttack(hitCharacter); // make sure to do it afterwards so you can check health
			}

			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_RangedGattlingAttack::SetupWaitInputTask()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_RangedGattlingAttack::AbilityInputPressed);
	WaitInputPress->ReadyForActivation();
}

void UGA_RangedGattlingAttack::AbilityInputPressed(float TimeWaited)
{
	SetupWaitInputTask();
	TryCommitAttack(FGameplayEventData());
}

void UGA_RangedGattlingAttack::TryCommitAttack(FGameplayEventData Payload)
{
	Fire(Payload);
}

void UGA_RangedGattlingAttack::StopAttacking(FGameplayEventData Payload)
{
	K2_EndAbility();
}
