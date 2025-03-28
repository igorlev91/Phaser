// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotSpecial.h"
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

UGA_DotSpecial::UGA_DotSpecial()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.speical.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.speical.activate"));
	//ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

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

	/*UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotSpecial::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();*/

	//UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	/*waitTargetDataTask->ValidData.AddDynamic(this, &UGA_DotSpecial::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_DotSpecial::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, TargetActorClass, spawnedTargetActor);
	ARTargetActor_DotSpecial* dotPickActor = Cast<ARTargetActor_DotSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetTargettingRadius(500.0f);
		dotPickActor->SetTargettingRange(5000.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);*/
}

void UGA_DotSpecial::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	/*if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
	}*/

	/*if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	for (TSubclassOf<UGameplayEffect>& damageEffect : AttackDamages)
	{
		FGameplayEffectSpecHandle damageSpec = MakeOutgoingGameplayEffectSpec(damageEffect, GetCurrentAbilitySpec()->Level);
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), damageSpec, Data);
	}
	SignalDamageStimuliEvent(Data);

	const FHitResult* blastLocationHitResult = Data.Get(1)->GetHitResult();
	if (blastLocationHitResult)
	{
		//ExecuteSpawnVFXCue();
	}
	GetOwningComponentFromActorInfo()->GetAnimInstance()->Montage_Play(CastingMontage);
	K2_EndAbility();*/
}

void UGA_DotSpecial::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	K2_EndAbility();
}