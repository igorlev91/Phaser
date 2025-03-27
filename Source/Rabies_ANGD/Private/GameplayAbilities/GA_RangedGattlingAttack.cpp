// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_RangedGattlingAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_RangedGattlingAttack::UGA_RangedGattlingAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_RangedGattlingAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitTargetAquiredEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitTargetAquiredEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::SendInputForHitScan);
	WaitTargetAquiredEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAttackTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_RangedGattlingAttack::StopAttacking);
	WaitStopEvent->ReadyForActivation();

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player)
	{
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos);
			});
	}

	SetupWaitInputTask();
}

void UGA_RangedGattlingAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Error, TEXT("Attacking cancelled"));
	if (ClientHitScanHandle.IsValid() && Player)
	{
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}


void UGA_RangedGattlingAttack::SendInputForHitScan(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (Player)
		{
			Player->Hitscan(4000);
			return;
		}
	}
}

void UGA_RangedGattlingAttack::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos)
{
	if (K2_HasAuthority())
	{
		if (hitActor == nullptr) return;
		if (hitActor != Player)
		{
			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RangedGattlingDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_RangedGattlingAttack::TryCommitAttack(FGameplayEventData Payload)
{
	SendInputForHitScan(Payload);
	bAttackCommitted = true;
}

void UGA_RangedGattlingAttack::AbilityInputPressed(float TimeWaited)
{
	SetupWaitInputTask();
	TryCommitAttack(FGameplayEventData());
}

void UGA_RangedGattlingAttack::SetupWaitInputTask()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_RangedGattlingAttack::AbilityInputPressed);
	WaitInputPress->ReadyForActivation();
}

void UGA_RangedGattlingAttack::StopAttacking(FGameplayEventData Payload)
{
	K2_EndAbility();
}
