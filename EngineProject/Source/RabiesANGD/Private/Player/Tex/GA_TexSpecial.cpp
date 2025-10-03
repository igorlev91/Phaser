// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexSpecial.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Player/Tex/RTargetActor_TexSpecial.h"

#include "Player/Tex/TexKnifesActor.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/Tex/RTex_SpecialProj.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_TexSpecial::UGA_TexSpecial()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());
}

void UGA_TexSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (Player->TexKnives)
	{
		Player->TexKnives->SetActiveState(false);
	}

	UAbilityTask_WaitGameplayEvent* sendOffAttack = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	sendOffAttack->EventReceived.AddDynamic(this, &UGA_TexSpecial::SendOffAttack);
	sendOffAttack->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PreparingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_TexSpecial::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_TexSpecial::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_TexSpecial* texPickActor = Cast<ARTargetActor_TexSpecial>(spawnedTargetActor);
	if (texPickActor)
	{
		texPickActor->SetOwningPlayerControler(Player->playerController, -12);
		texPickActor->SetTargettingRadus(500.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, texPickActor);

	for (int i = 1; i < 6; ++i)
	{
		float offset = 4.0f * (i - 3);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwningActorFromActorInfo();

		ARTargetActor_TexSpecial* extraActor = GetWorld()->SpawnActor<ARTargetActor_TexSpecial>(targetActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		TexAbilityHints.Add(extraActor);


		if (extraActor)
		{
			extraActor->SetOwningPlayerControler(Player->playerController, offset);
			extraActor->SetTargettingRadus(500.0f);
		}
	}
}

void UGA_TexSpecial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	for (ARTargetActor_TexSpecial* texAbility : TexAbilityHints)
	{
		texAbility->Destroy();
	}
	TexAbilityHints.Empty();

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetCooldownRefreshTag(), FGameplayEventData());
}

void UGA_TexSpecial::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (playTargettingMontageTask)
	{
		playTargettingMontageTask->OnBlendOut.RemoveAll(this);
		playTargettingMontageTask->OnInterrupted.RemoveAll(this);
		playTargettingMontageTask->OnCompleted.RemoveAll(this);
		playTargettingMontageTask->OnCancelled.RemoveAll(this);
	}

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 100);
		}
	}

	for (ARTargetActor_TexSpecial* texAbility : TexAbilityHints)
	{
		texAbility->Destroy();
	}
	TexAbilityHints.Empty();

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			K2_EndAbility();
			return;
		}
	}

	directionRot = 10.0f;

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_TexSpecial::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	GetWorld()->GetTimerManager().SetTimer(DisappearKnivesTimer, this, &UGA_TexSpecial::DisappearKnives, 0.2f, false);

	//K2_EndAbility();
}

void UGA_TexSpecial::DisappearKnives()
{
	if (Player->TexKnives)
	{

		Player->TexKnives->SetActiveState(true);
	}
}


void UGA_TexSpecial::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}

void UGA_TexSpecial::SendOffAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		//UE_LOG(LogTemp, Error, TEXT("Sending Attack"));
		FVector viewLoc;
		FRotator viewRot;

		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);
		viewRot.Yaw -= directionRot;
		ARTex_SpecialProj* newProjectile = GetWorld()->SpawnActor<ARTex_SpecialProj>(TexProjectile, Player->GetActorLocation(), viewRot);
		newProjectile->Init(AttackDamages, CooldownReduction);
		newProjectile->InitOwningCharacter(Player);
		newProjectile->SetOwner(Player);
		directionRot -= 5.0f;
		TriggerAudioCue();
	}
}