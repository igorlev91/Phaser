// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_PlayerMelee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_PlayerMelee::UGA_PlayerMelee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_PlayerMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_PlayerMelee::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_PlayerMelee::HandleDamage);
	WaitForDamage->ReadyForActivation();
}

void UGA_PlayerMelee::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		ARCharacterBase* character = Cast<ARCharacterBase>(GetOwningActorFromActorInfo());
		if (character)
		{
			character->ServerPlayAnimMontage(AttackAnim);
		}
	}
}

void UGA_PlayerMelee::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}
