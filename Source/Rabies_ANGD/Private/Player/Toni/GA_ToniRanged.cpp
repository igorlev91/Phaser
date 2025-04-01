// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/GA_ToniRanged.h"
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

#include "Enemy/REnemyBase.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ToniRanged::UGA_ToniRanged()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_ToniRanged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	bool bFound = false;
	//float attackCooldown = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetRangedAttackCooldownReductionAttribute(), bFound);
	float attackSpeed = 1.0f;

	if (bFound == true)
	{
		//attackSpeed = 3.0f - 2.0f * (attackCooldown - 0.1f) / (1.0f - 0.1f);
		//UE_LOG(LogTemp, Error, TEXT("Attack Speed: %f"), attackSpeed);
	}

	UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LeftShootingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ToniRanged::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ToniRanged::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ToniRanged::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ToniRanged::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	TriggerAudioCue();
	Fire();
}

void UGA_ToniRanged::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}

void UGA_ToniRanged::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	if (K2_HasAuthority())
	{
		if (hitActor == nullptr) return;
		if (hitActor != Player)
		{
			ARCharacterBase* hitEnemy = Cast<ARCharacterBase>(hitActor);
			if (hitEnemy == nullptr)
				return;

			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RangedDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

			if (ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(hitActor))
				Player->HitRangedAttack(hitCharacter); // make sure to do it afterwards so you can check health

			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_ToniRanged::Fire()
{
	if (K2_HasAuthority())
	{
		if (Player)
		{
			Player->Hitscan(6000, Player->GetPlayerBaseState());
		}
	}
}
