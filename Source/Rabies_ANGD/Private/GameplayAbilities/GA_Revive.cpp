// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_Revive.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Framework/EOSPlayerState.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"

UGA_Revive::UGA_Revive()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetRevivingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetUnActionableTag());
	CancelAbilitiesWithTag.AddTag(URAbilityGenericTags::GetInvisTag());
}

void UGA_Revive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitTargetAquiredEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndRevivingTag());
	WaitTargetAquiredEvent->EventReceived.AddDynamic(this, &UGA_Revive::StopHoldingRevive);
	WaitTargetAquiredEvent->ReadyForActivation();

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	GetWorld()->GetTimerManager().ClearTimer(ReviveHandle);
	CurrentHoldDuration = 0;

	if (!Player->GetCharacterMovement() || Player->GetCharacterMovement()->IsFalling()) return;

	Player->playerController->ChangeRevivalState(true, 0);

	ReviveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_Revive::Hold, CurrentHoldDuration));
}

void UGA_Revive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// this gets called when K2_EndAbility();

	Player->playerController->ChangeRevivalState(false, 0);
	GetWorld()->GetTimerManager().ClearTimer(ReviveHandle);
}

void UGA_Revive::Hold(float timeRemaining)
{
	if (Player)
	{
		if (Player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		{
			Player->playerController->ChangeRevivalState(false, 0);
			K2_EndAbility();
		}
	}

	if (CurrentHoldDuration <= 5)
	{
		CurrentHoldDuration += (GetWorld()->GetDeltaSeconds() * Player->GetReviveSpeed());
		bool visible = (CurrentHoldDuration >= 1 / 8.0f) ? true : false;
		Player->playerController->ChangeRevivalState(visible, CurrentHoldDuration);
		ReviveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_Revive::Hold, CurrentHoldDuration));
		return;
	}
	else
	{
		TArray<AActor*> playersRevived = Player->nearbyFaintedActors;
		EndHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_Revive::EndDelay));
		AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();

		for (AActor* player : playersRevived)
		{
			//FGameplayEventData Payload = FGameplayEventData();
			//Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(EOSPlayeState->GetPlayer());

			//FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(ReviveEffectClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

			const TArray<APlayerState*>& PlayerArray = GameState->PlayerArray;

			ARPlayerBase* playerBase = Cast<ARPlayerBase>(player);
            // Iterate through the PlayerArray
            for (APlayerState* PlayerState : PlayerArray)
            {
				AEOSPlayerState* EOSPlayeState = Cast<AEOSPlayerState>(PlayerState);
                if (EOSPlayeState && playerBase)
                {
					if (EOSPlayeState->GetPlayer() == playerBase)
					{
						playerBase->SetPlayerReviveState(false);
						Player->playerController->Server_RequestRevive(EOSPlayeState);
						Player->ServerPlayAnimMontage(Player->RevivingBuddy);
						UE_LOG(LogTemp, Error, TEXT("%s Reviving"), *player->GetName());
					}
                }
            }
		}
		Player->bInRangeToRevive = false;
		//process revive
	}
}

void UGA_Revive::StopHoldingRevive(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_Revive::EndDelay()
{
	K2_EndAbility();
}
