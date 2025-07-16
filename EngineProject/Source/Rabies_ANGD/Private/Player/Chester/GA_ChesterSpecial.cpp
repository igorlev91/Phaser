// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/GA_ChesterSpecial.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/RPlayerController.h"
#include "Player/RPlayerBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/EOSActionGameState.h"
#include "NiagaraSystem.h"
#include "Player/Chester/RTargetActor_ChesterSpecial.h"

#include "GameFramework/CharacterMovementComponent.h"

UGA_ChesterSpecial::UGA_ChesterSpecial()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());
}

void UGA_ChesterSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	bStopRunning = false;

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ChesterSpecial::HandleDamage);
	WaitForDamage->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForStopRunning = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetExpTag());
	WaitForStopRunning->EventReceived.AddDynamic(this, &UGA_ChesterSpecial::StopRunning);
	WaitForStopRunning->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_ChesterSpecial::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_ChesterSpecial::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_ChesterSpecial* dotPickActor = Cast<ARTargetActor_ChesterSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetOwningPlayerControler(Player->playerController, true);
		dotPickActor->SetTargettingRadus(500.0f);
		dotPickActor->SetTargettingRange(5000.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningActorFromActorInfo();
	ChesterAbilityHint = GetWorld()->SpawnActor<ARTargetActor_ChesterSpecial>(targetActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (ChesterAbilityHint)
	{
		ChesterAbilityHint->SetOwningPlayerControler(Player->playerController, false);
		ChesterAbilityHint->SetTargettingRadus(500.0f);
		ChesterAbilityHint->SetTargettingRange(5000.0f);
	}
}

void UGA_ChesterSpecial::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		bool bFound = false;
		float strength = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackStrengthAttribute(), bFound);

		if (bFound == false)
			return;

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
							FVector LaunchVelocity = Player->GetActorForwardVector() * (1500.f + (strength * 20.0));  // Adjust strength as needed
							LaunchVelocity.Z += (400.0f + (strength * 10));
							hitCharacter->LaunchBozo(LaunchVelocity);
							SignalDamageStimuliEvent(Payload.TargetData);

							Player->HitSpecialAttack(hitCharacter);
						}
					}
				}
			}
		}
	}
}

void UGA_ChesterSpecial::StopRunning(FGameplayEventData Payload)
{
	bStopRunning = true;

	TriggerAudioCue();
	FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(SlowdownClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	SlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
}

void UGA_ChesterSpecial::RunForward()
{
	if (bStopRunning)
		return;

	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetImmuneTag());

	FVector frontFacingVector = Player->GetActorForwardVector();

	Player->GetMovementComponent()->AddInputVector(frontFacingVector * 1000.0f, true);

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ChesterSpecial::RunForward));

}

void UGA_ChesterSpecial::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
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
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	if (ChesterAbilityHint != nullptr)
	{
		ChesterAbilityHint->Destroy();
	}

	FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(RunningClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	//pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), 500.0f);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Multicast_RequestSpawnVFXOnCharacter(ShootingParticle, Player, Player->GetActorLocation(), Player->GetActorForwardVector(), 0);
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

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MeleeAttackAnim);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_ChesterSpecial::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetImmuneTag());

	GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &UGA_ChesterSpecial::RunForward, 0.1f, false);

	//K2_EndAbility();
}

void UGA_ChesterSpecial::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}

void UGA_ChesterSpecial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ChesterAbilityHint != nullptr)
	{
		ChesterAbilityHint->Destroy();
	}

	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetImmuneTag());

	//StopDurationAudioEffect();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(SlowDownEffectHandle);
	}
}