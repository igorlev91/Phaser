// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/GA_ChesterUltimate.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/Chester/RChester_UltimateProj.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"
#include "Targeting/RTargetActor_DotSpecial.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ChesterUltimate::UGA_ChesterUltimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_ChesterUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(SlowdownClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		SlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
		//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
	}

	UAbilityTask_WaitGameplayEvent* sendOffAttack = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateAttackActivationTag());
	sendOffAttack->EventReceived.AddDynamic(this, &UGA_ChesterUltimate::SendOffAttack);
	sendOffAttack->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_ChesterUltimate::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_ChesterUltimate::TargetCancelled);
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

void UGA_ChesterUltimate::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
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
	}

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_ChesterUltimate::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	//K2_EndAbility();
}

void UGA_ChesterUltimate::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}

void UGA_ChesterUltimate::SendOffAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		//UE_LOG(LogTemp, Error, TEXT("Sending Attack"));
		FVector viewLoc;
		FRotator viewRot;

		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);
		FVector SpawnLoc = Player->GetActorLocation();
		ARChester_UltimateProj* newProjectile = GetWorld()->SpawnActor<ARChester_UltimateProj>(ChesterProjectile, SpawnLoc, viewRot);
		newProjectile->Init(AttackDamages, Player);
		newProjectile->InitOwningCharacter(Player);
		newProjectile->SetOwner(Player);
		StartDurationAudioEffect();
	}
}

void UGA_ChesterUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	StopDurationAudioEffect();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(SlowDownEffectHandle);
	}
}
