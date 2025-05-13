// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/GA_BoltHead_Special.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_BoltHead_Special::UGA_BoltHead_Special()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_BoltHead_Special::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		K2_EndAbility();
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	Player->ServerPlay_Torso_AnimMontage(TargettingMontage, 1.0f);

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_BoltHead_Special::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_BoltHead_Special::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_BoltHead_Special::HandleDamage);
	WaitForDamage->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_DotSpecial* dotPickActor = Cast<ARTargetActor_DotSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetOwningPlayerControler(Player->playerController);
		dotPickActor->SetTargettingRadus(500.0f);
		dotPickActor->SetTargettingRange(5000.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);

	GetWorld()->GetTimerManager().ClearTimer(PunchHandle);
	CurrentHoldDuration = 0.0f;

	PunchHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Special::Hold, CurrentHoldDuration));

}

void UGA_BoltHead_Special::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	Player->playerController->ChangeSuperPunchState(false, 0);
}

void UGA_BoltHead_Special::Hold(float timeRemaining)
{
	if (CurrentHoldDuration <= 2)
	{
		CurrentHoldDuration += GetWorld()->GetDeltaSeconds();
		Player->playerController->ChangeSuperPunchState(true, CurrentHoldDuration);

		PunchHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Special::Hold, CurrentHoldDuration));
	}
}

void UGA_BoltHead_Special::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			Player->playerController->ChangeSuperPunchState(false, 0);
			K2_EndAbility();
			return;
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(PunchHandle);

	FVector viewLoc;
	FRotator viewRot;

	Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

	float charge = CurrentHoldDuration * 60.0f;
	FVector LaunchVelocity = viewRot.Vector() * (1500.f + (charge * 60.0));  // Adjust strength as needed
	Player->LaunchBozo(LaunchVelocity);

	GetWorld()->GetTimerManager().SetTimer(DelayEndHandle, this, &UGA_BoltHead_Special::DelayEnd, 1.0f, false);

	Player->ServerPlay_Torso_AnimMontage(CastingMontage, 1.0f);
}

void UGA_BoltHead_Special::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamages[0], GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		for (int32 i = 0; i < Payload.TargetData.Num(); ++i)
		{
			const FGameplayAbilityTargetData* Data = Payload.TargetData.Get(i);
			if (Data)
			{
				const TArray<TWeakObjectPtr<AActor>> Actors = Data->GetActors();
				for (const TWeakObjectPtr<AActor>& WeakActorPtr : Actors)
				{
					if (WeakActorPtr.IsValid())
					{
						ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(WeakActorPtr.Get());
						if (hitCharacter)
						{
							ARPlayerBase* hitPlayer = Cast<ARPlayerBase>(hitCharacter);
							if (hitPlayer)
							{
								return;
							}

							Player->HitSpecialAttack(hitCharacter);
						}
					}
				}
			}
		}
	}
}

void UGA_BoltHead_Special::DelayEnd()
{
	Player->playerController->ChangeSuperPunchState(false, 0);
	K2_EndAbility();
}

void UGA_BoltHead_Special::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}