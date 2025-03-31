// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotUltimate.h"
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

UGA_DotUltimate::UGA_DotUltimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_DotUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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


	UAbilityTask_WaitGameplayEvent* sendOffAttack = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	sendOffAttack->EventReceived.AddDynamic(this, &UGA_DotUltimate::SendOffAttack);
	sendOffAttack->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? TargettingMontageAir : TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	/*UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_DotUltimate::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_DotUltimate::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();*/

	/*AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_DotSpecial* dotPickActor = Cast<ARTargetActor_DotSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetOwningPlayerControler(Player->playerController);
		dotPickActor->SetTargettingRadus(500.0f);
		dotPickActor->SetTargettingRange(5000.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);*/
}

void UGA_DotUltimate::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (playTargettingMontageTask)
	{
		playTargettingMontageTask->OnBlendOut.RemoveAll(this);
		playTargettingMontageTask->OnInterrupted.RemoveAll(this);
		playTargettingMontageTask->OnCompleted.RemoveAll(this);
		playTargettingMontageTask->OnCancelled.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			K2_EndAbility();
			return;
		}
	}

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? CastingMontageAir : CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();
}

void UGA_DotUltimate::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
}

void UGA_DotUltimate::SendOffAttack(FGameplayEventData Payload)
{
	FVector viewLoc;
	FRotator viewRot;

	/*
	Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);
	ARDot_SpecialProj* newProjectile = GetWorld()->SpawnActor<ARDot_SpecialProj>(DotProjectile, Player->GetActorLocation(), viewRot);
	newProjectile->Init(AttackDamages);
	newProjectile->InitOwningCharacter(Player);
	newProjectile->SetOwner(Player);*/
}
