// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotSpecial.h"
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

UGA_DotSpecial::UGA_DotSpecial()
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

void UGA_DotSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	sendOffAttack->EventReceived.AddDynamic(this, &UGA_DotSpecial::SendOffAttack);
	sendOffAttack->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? TargettingMontageAir : TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_DotSpecial::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_DotSpecial::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

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
}

void UGA_DotSpecial::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (playTargettingMontageTask)
	{
		playTargettingMontageTask->OnBlendOut.RemoveAll(this);
		playTargettingMontageTask->OnInterrupted.RemoveAll(this);
		playTargettingMontageTask->OnCompleted.RemoveAll(this);
		playTargettingMontageTask->OnCancelled.RemoveAll(this);
	}

	//UE_LOG(LogTemp, Error, TEXT("Target aquired!"));
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			K2_EndAbility();
			return;
		}

		/*for (TSubclassOf<UGameplayEffect>& damageEffect : AttackDamages)
		{
			FGameplayEffectSpecHandle damageSpec = MakeOutgoingGameplayEffectSpec(damageEffect, GetCurrentAbilitySpec()->Level);
			ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), damageSpec, Data);
		}
		SignalDamageStimuliEvent(Data);*/
		//SignalDamageStimuliEvent(Data);
	}

	/*const FHitResult* blastLocationHitResult = Data.Get(1)->GetHitResult();
	if (blastLocationHitResult)
	{
		//ExecuteSpawnVFXCue();
	}*/

	//UE_LOG(LogTemp, Error, TEXT("Doing Anim"));
	//Player->ServerPlayAnimMontage(CastingMontage);
	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? CastingMontageAir : CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	//K2_EndAbility();
}


void UGA_DotSpecial::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}

void UGA_DotSpecial::SendOffAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		//UE_LOG(LogTemp, Error, TEXT("Sending Attack"));
		FVector viewLoc;
		FRotator viewRot;

		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);
		ARDot_SpecialProj* newProjectile = GetWorld()->SpawnActor<ARDot_SpecialProj>(DotProjectile, Player->GetActorLocation(), viewRot);
		newProjectile->Init(AttackDamages);
		newProjectile->InitOwningCharacter(Player);
		newProjectile->SetOwner(Player);
		TriggerAudioCue();
	}
}
