// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/GA_BoltHead_Passive.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Framework/EOSPlayerState.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/EOSActionGameState.h"
#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Player/BoltHead/RBoltHead_Head.h"
#include "Player/BoltHead/RBoltHead_Actor.h"

UGA_BoltHead_Passive::UGA_BoltHead_Passive()
{
}

void UGA_BoltHead_Passive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	bReboot = true;

	TArray<AActor*> AttachedActors;
	Player->GetAttachedActors(AttachedActors);

	for (AActor* actor : AttachedActors)
	{
		ARBoltHead_Actor* boltHeadActor = Cast<ARBoltHead_Actor>(actor);
		if (boltHeadActor)
		{
			BoltHead = boltHeadActor;
			if (BoltHead)
			{
				BoltHead->owningPlayer = Player;

				bBusy = false;
				DeathCheckerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::Checker));

				/*if(BoltHead->GetAnimInstance())
				{

				}*/
			}
		}
	}
}

void UGA_BoltHead_Passive::Checker()
{
	DeathCheckerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::Checker));

	if (bBusy)
		return;

	if (Player == nullptr)
		return;

	if (Player->playerController == nullptr)
		return;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(Player->playerController->GetWorld(), ARPlayerBase::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (bBusy == true)
			continue;

		ARPlayerBase* allyPlayer = Cast<ARPlayerBase>(Actor);

		if (!allyPlayer)
			continue;

		if (allyPlayer == Player && bBusy == false)
		{
			if(bReboot == false)
				continue;

			if ((allyPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == true))
			{
				if (Player->GetPlayerBaseState())
				{
					bReboot = false;
					Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					Player->SetPlayerReviveState(false);
					Player->playerController->Server_RequestRevive(Player->GetPlayerBaseState());

					AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
					if (gameState)
					{
						gameState->Server_RequestSpawnVFXOnCharacter(DeathParticle, Player, Player->GetActorLocation(), Player->GetActorLocation(), 0);
					}

					continue;
				}
			}
		}

		if (allyPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == true)
		{
			bBusy = true;
			WeeWoo(allyPlayer, 0.0f);
			continue;
		}
	}
}

void UGA_BoltHead_Passive::WeeWoo(ARPlayerBase* damagedPlayer, float reviveProgress)
{
	if (BoltHead && Player)
	{
		bool bFound = false;
		float movementSpeed = 700.0f;
		movementSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMovementSpeedAttribute(), bFound);

		bool bFoundRevive = false;
		float reviveSpeed = 1.0f;
		reviveSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetReviveSpeedAttribute(), bFound);

		if (damagedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == false || damagedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullyDeadTag()) == true)
		{
			bBusy = false;
			GoHomeHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::GoHome));
			return;
		}

		BoltHead->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		const float Distance = FVector::Dist(BoltHead->GetActorLocation(), damagedPlayer->GetActorLocation());
		const float StopThreshold = 100.f;

		if (Distance <= StopThreshold)
		{
			reviveProgress += GetWorld()->GetDeltaSeconds() * reviveSpeed;
			BoltHead->ServerPlay_Head_AnimMontage(healingMontage, reviveSpeed);
			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState)
			{
				gameState->Multicast_ShootTexUltimate(HealingParticle, BoltHead, BoltHead->GetActorLocation(), damagedPlayer->GetActorLocation(), damagedPlayer->GetActorLocation());
			}

			if (reviveProgress >= 10.0f)
			{
				AEOSPlayerState* EOSPlayeState = damagedPlayer->GetPlayerBaseState();
				if (EOSPlayeState && Player)
				{
					damagedPlayer->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					damagedPlayer->SetPlayerReviveState(false);
					Player->playerController->Server_RequestRevive(EOSPlayeState);
				} // send it home
				bBusy = false;
				GoHomeHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::GoHome));
				return;
			}
		}
		else
		{
			FVector NewLocation = FMath::VInterpTo(BoltHead->GetActorLocation(), damagedPlayer->GetActorLocation(), GetWorld()->GetDeltaSeconds(), (movementSpeed * 0.0005f));
			BoltHead->SetActorLocation(NewLocation);

			FRotator TargetRotation = (damagedPlayer->GetActorLocation() - NewLocation).Rotation();
			TargetRotation.Yaw -= 90.0f;
			FRotator SmoothedRotation = FMath::RInterpTo(BoltHead->GetHeadMesh()->GetComponentRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
			BoltHead->GetHeadMesh()->SetWorldRotation(SmoothedRotation);

		}

		FinishedHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::WeeWoo, damagedPlayer, reviveProgress));
	}
}

void UGA_BoltHead_Passive::GoHome()
{
	if (BoltHead && Player)
	{
		bool bFound = false;
		float movementSpeed = 700.0f;
		movementSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMovementSpeedAttribute(), bFound);

		const float Distance = FVector::Dist(BoltHead->GetActorLocation(), Player->GetActorLocation());
		const float StopThreshold = 100.f;

		if (Distance <= StopThreshold)
		{
			BoltHead->AttachToComponent(Player->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
			BoltHead->SetActorRelativeLocation(FVector(10, 0, 20));
			BoltHead->SetActorRelativeRotation(FRotator(0, -90, 0));

			return;
		}
		else
		{
			FVector NewLocation = FMath::VInterpTo(BoltHead->GetActorLocation(), Player->GetActorLocation(), GetWorld()->GetDeltaSeconds(), (movementSpeed * 0.002f));
			BoltHead->SetActorLocation(NewLocation);

			FRotator TargetRotation = (Player->GetActorLocation() - NewLocation).Rotation();
			TargetRotation.Yaw -= 90.0f;
			FRotator SmoothedRotation = FMath::RInterpTo(BoltHead->GetHeadMesh()->GetComponentRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f);
			BoltHead->GetHeadMesh()->SetWorldRotation(SmoothedRotation);
		}

		GoHomeHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::GoHome));
	}
}

void UGA_BoltHead_Passive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}