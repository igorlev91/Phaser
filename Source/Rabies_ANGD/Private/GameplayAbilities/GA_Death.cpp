// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_Death.h"


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

UGA_Death::UGA_Death()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetDeadTag());
}

void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Applied Dead"));

	UAbilityTask_WaitGameplayEvent* EndTakeOffEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetReviveTag());
	EndTakeOffEvent->EventReceived.AddDynamic(this, &UGA_Death::StopDeath);
	EndTakeOffEvent->ReadyForActivation();

	TriggerAudioCue();
}

void UGA_Death::StopDeath(FGameplayEventData Payload)
{
	K2_EndAbility();
}
