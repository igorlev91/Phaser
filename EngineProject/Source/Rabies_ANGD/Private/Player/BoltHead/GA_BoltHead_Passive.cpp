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

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Player/BoltHead/RBoltHead_Head.h"

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

	TArray<USkeletalMeshComponent*> SkeletalComponents;
	Player->GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	for (USkeletalMeshComponent* Skeletal : SkeletalComponents)
	{
		if (Skeletal)
		{
			if (Skeletal->GetName() == TEXT("Head"))
			{
				BoltHead = Cast<URBoltHead_Head>(Skeletal);
				if (BoltHead)
				{
					bBusy = false;
					DeathCheckerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::Checker));

					if (BoltHead->GetAnimInstance())
					{

					}
				}
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
		ARPlayerBase* allyPlayer = Cast<ARPlayerBase>(Actor);

		if (!allyPlayer)
			continue;

		if (allyPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == true)
		{
			bBusy = true;
			WeeWoo(allyPlayer);
			return;
		}
	}
}

void UGA_BoltHead_Passive::WeeWoo(ARPlayerBase* damagedPlayer)
{
	if (BoltHead && Player)
	{
		bool bFound = false;
		float movementSpeed = 700.0f;
		movementSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMovementSpeedAttribute(), bFound);

		bool bFoundRevive = false;
		float reviveSpeed = 1.0f;
		reviveSpeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetReviveSpeedAttribute(), bFound);

		if (damagedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == false)
		{
			return;
		}

		BoltHead->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		FVector CurrentLocation = BoltHead->GetComponentLocation();

		FVector NewLocation = FMath::VInterpTo(CurrentLocation, damagedPlayer->GetActorLocation(), GetWorld()->GetDeltaSeconds(), (movementSpeed * 0.001f));
		BoltHead->SetWorldLocation(NewLocation);


		FinishedHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Passive::WeeWoo, damagedPlayer));
	}
}


void UGA_BoltHead_Passive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}