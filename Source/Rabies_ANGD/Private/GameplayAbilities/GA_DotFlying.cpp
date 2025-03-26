// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotFlying.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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

UGA_DotFlying::UGA_DotFlying()
{

}

void UGA_DotFlying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	UAbilityTask_WaitGameplayEvent* EndFlyingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndFlyingTag());
	EndFlyingEvent->EventReceived.AddDynamic(this, &UGA_DotFlying::StopFlying);
	EndFlyingEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* EndTakeOffEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndTakeOffChargeTag());
	EndTakeOffEvent->EventReceived.AddDynamic(this, &UGA_DotFlying::StopTakeOff);
	EndTakeOffEvent->ReadyForActivation();

	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
	CurrentHoldDuration = 0;

	if (!Player->GetCharacterMovement() || Player->GetCharacterMovement()->IsFalling()) return;

	Player->playerController->ChangeTakeOffState(true, 0);
	TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
}

void UGA_DotFlying::StopFlying(FGameplayEventData Payload)
{
	URAbilitySystemComponent* AbilitySystem = Cast<URAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (AbilitySystem)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = FlyingSpeedClass;

		const TArray<FActiveGameplayEffectHandle>& ActiveEffects = AbilitySystem->GetActiveEffects(Query);
		if (ActiveEffects.Num() > 0)
		{
			AbilitySystem->RemoveActiveGameplayEffect(ActiveEffects[0]);
		}
	}

	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
	Player->playerController->ChangeTakeOffState(false, 0);
	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);

	K2_EndAbility();
}

void UGA_DotFlying::StopTakeOff(FGameplayEventData Payload)
{
	Player->playerController->ChangeTakeOffState(false, 0);
	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
}

void UGA_DotFlying::Hold(float timeRemaining)
{
	if (CurrentHoldDuration <= 1)
	{
		CurrentHoldDuration += GetWorld()->GetDeltaSeconds();
		bool visible = (CurrentHoldDuration >= 1 / 8.0f) ? true : false;
		Player->playerController->ChangeTakeOffState(visible, CurrentHoldDuration);
		TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
	}
	else
	{
		Player->playerController->ChangeTakeOffState(false, 0);
		GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(FlyingSpeedClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

		Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
		Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
		Player->PlayAnimMontage(TakeOffMontage);
	}
}
