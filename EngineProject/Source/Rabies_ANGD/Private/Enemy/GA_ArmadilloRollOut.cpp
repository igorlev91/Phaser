// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_ArmadilloRollOut.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Components/AudioComponent.h"
#include "Player/RPlayerBase.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "AI/REnemyAIController.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ArmadilloRollOut::UGA_ArmadilloRollOut()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
}

void UGA_ArmadilloRollOut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Enemy = Cast<AREnemyBase>(GetOwningActorFromActorInfo());
	if (Enemy == nullptr)
		return;

	EnemyAI = Cast<AAIController>(Enemy->GetController());
	if (EnemyAI == nullptr)
		return;

	TArray<USkeletalMeshComponent*> SkeletalComponents;
	Enemy->GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	for (USkeletalMeshComponent* Skeletal : SkeletalComponents)
	{
		if (Skeletal)
		{
			if (Skeletal->GetName() == TEXT("Roll"))
			{
				RollerSkeletalMesh = Skeletal;
			}
		}
	}

	if (RollerSkeletalMesh == nullptr)
		return;

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_ArmadilloRollOut::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamageRollOut = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetExpTag());
	WaitForDamageRollOut->EventReceived.AddDynamic(this, &UGA_ArmadilloRollOut::HandleRollOutDamage);
	WaitForDamageRollOut->ReadyForActivation();
}

void UGA_ArmadilloRollOut::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (Enemy)
		{
			UObject* target = EnemyAI->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
			TargetActor = Cast<AActor>(target);

			if (TargetActor == nullptr)
			{
				return;
			}

			Enemy->ServerPlayOtherSkeletalMeshAnimMontage(RollerSkeletalMesh, Ball_RollIntoBall);

			PlayAudioLine(RollUp, false);

			playRollIntoBallMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Basic_RollIntoBall);
			playRollIntoBallMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRollingIntoBall);
			playRollIntoBallMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRollingIntoBall);
			playRollIntoBallMontageTask->OnCompleted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRollingIntoBall);
			playRollIntoBallMontageTask->OnCancelled.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRollingIntoBall);
			playRollIntoBallMontageTask->ReadyForActivation();
		}
	}
}

void UGA_ArmadilloRollOut::FinishRollingIntoBall()
{
	if (K2_HasAuthority())
	{
		if (Enemy && TargetActor)
		{
			Enemy->ServerPlayOtherSkeletalMeshAnimMontage(RollerSkeletalMesh, Ball_RevUp);

			if (EnemyAI && EnemyAI->GetBlackboardComponent())
			{
				EnemyAI->GetBlackboardComponent()->SetValueAsBool(RollingBlackboardKeyName, true);
			}

			PlayAudioLine(RevUp, false);

			FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(RollingClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			SpeedupEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);

			playRevUpMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Basic_RevUp);
			playRevUpMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRevingUp);
			playRevUpMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRevingUp);
			playRevUpMontageTask->OnCompleted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRevingUp);
			playRevUpMontageTask->OnCancelled.AddDynamic(this, &UGA_ArmadilloRollOut::FinishRevingUp);
			playRevUpMontageTask->ReadyForActivation();
		}
	}
}

void UGA_ArmadilloRollOut::FinishRevingUp()
{
	if (K2_HasAuthority())
	{
		if (Enemy && TargetActor)
		{
			Enemy->ServerPlayOtherSkeletalMeshAnimMontage(RollerSkeletalMesh, Ball_RollLoop);

			bStopRolling = false;
			PlayAudioLine(Rolling, true);

			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ArmadilloRollOut::RollForward));


			playRollLoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Basic_RollLoop);
			playRollLoopMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArmadilloRollOut::ExhaustedRolling);
			playRollLoopMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArmadilloRollOut::ExhaustedRolling);
			playRollLoopMontageTask->OnCompleted.AddDynamic(this, &UGA_ArmadilloRollOut::ExhaustedRolling);
			playRollLoopMontageTask->OnCancelled.AddDynamic(this, &UGA_ArmadilloRollOut::ExhaustedRolling);
			playRollLoopMontageTask->ReadyForActivation();
		}
	}
}

void UGA_ArmadilloRollOut::RollForward()
{
	if (bStopRolling)
		return;

	if (Enemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Enemy->GetOwner(), URAbilityGenericTags::GetUnrollTag(), FGameplayEventData());
		FinishAbility();
		return;
	}

	if (TargetActor)
	{
		FRotator CurrentRotation = Enemy->GetActorRotation();
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();

		FRotator TargetRotation = DirectionToTarget.Rotation();

		float RotationSpeed = 2.5f;
		TargetRotation.Pitch = 0;
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

		Enemy->SetActorRotation(NewRotation);
	}

	FVector frontFacingVector = Enemy->GetActorForwardVector();

	Enemy->GetMovementComponent()->AddInputVector(frontFacingVector * 1000.0f, true);

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ArmadilloRollOut::RollForward));
}

void UGA_ArmadilloRollOut::ExhaustedRolling()
{
	if (K2_HasAuthority())
	{
		if (Enemy && TargetActor)
		{
			bStopRolling = true;

			Enemy->ServerPlayOtherSkeletalMeshAnimMontage(RollerSkeletalMesh, Ball_RollToDizzy);

			playRollToDizzyMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Basic_RollToDizzy);
			playRollToDizzyMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToDizzyMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToDizzyMontageTask->OnCompleted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToDizzyMontageTask->OnCancelled.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToDizzyMontageTask->ReadyForActivation();
		}
	}

}

void UGA_ArmadilloRollOut::HitCollide()
{
	if (K2_HasAuthority())
	{
		if (Enemy)
		{
			PlayAudioLine(Crash, false);

			bStopRolling = true;

			Enemy->ServerPlayOtherSkeletalMeshAnimMontage(RollerSkeletalMesh, Ball_RollToCollide);

			playRollToCollideMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Basic_RollToCollide);
			playRollToCollideMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToCollideMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToCollideMontageTask->OnCompleted.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToCollideMontageTask->OnCancelled.AddDynamic(this, &UGA_ArmadilloRollOut::FinishAbility);
			playRollToCollideMontageTask->ReadyForActivation();
		}
	}
}

void UGA_ArmadilloRollOut::FinishAbility()
{
	if (EnemyAI && EnemyAI->GetBlackboardComponent())
	{
		EnemyAI->GetBlackboardComponent()->SetValueAsBool(RollingBlackboardKeyName, false);
	}

	AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (GameState)
	{
		GameState->Multicast_StopComponentRolling(Enemy);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(SpeedupEffectHandle);
	}
}

void UGA_ArmadilloRollOut::HandleRollOutDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
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
								FVector LaunchVelocity = Enemy->GetActorForwardVector() * (1500.f);  // Adjust strength as needed
								LaunchVelocity.Z += (400.0f);
								hitCharacter->LaunchBozo(LaunchVelocity);
							}
							else
							{
								AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitCharacter);
								if (hitEnemy)
								{
									return;
								}
							}
						}
					}
				}
			}
		}

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(CollideDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		HitCollide();
	}
}

void UGA_ArmadilloRollOut::PlayAudioLine(USoundBase* soundEffect, bool bIsRolling)
{
	if (K2_HasAuthority())
	{
		AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (GameState)
		{
			FVector Location = Enemy->GetActorLocation();
			FRotator Rotation = FRotator::ZeroRotator;
			USoundAttenuation* Attenuation = nullptr;

			if (bIsRolling)
			{
				GameState->Multicast_RequestPlayAudioComponentRolling(soundEffect, Enemy, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
			}
			else
			{
				GameState->Multicast_RequestPlayAudio(soundEffect, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
			}

		}

	}
}

void UGA_ArmadilloRollOut::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}