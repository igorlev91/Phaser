// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexPassive.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Framework/EOSPlayerState.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UGA_TexPassive::UGA_TexPassive()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetInvisTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetInvisTag());
}

void UGA_TexPassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(BonusStatsClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	StatsEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	// apply the material and listen for when you basic attack

	UAbilityTask_WaitGameplayEvent* specialActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	specialActivation->EventReceived.AddDynamic(this, &UGA_TexPassive::FinishStealth);
	specialActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* ultimateActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateAttackActivationTag());
	ultimateActivation->EventReceived.AddDynamic(this, &UGA_TexPassive::FinishStealth);
	ultimateActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* meleeActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	meleeActivation->EventReceived.AddDynamic(this, &UGA_TexPassive::FinishStealth);
	meleeActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* rangedActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackStrengthTag());
	rangedActivation->EventReceived.AddDynamic(this, &UGA_TexPassive::FinishStealth);
	rangedActivation->ReadyForActivation();

}

void UGA_TexPassive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(StatsEffectHandle);
	}
}

void UGA_TexPassive::FinishStealth(FGameplayEventData Payload)
{
	K2_EndAbility();
}
