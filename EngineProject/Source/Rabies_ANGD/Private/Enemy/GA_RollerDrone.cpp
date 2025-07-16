// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_RollerDrone.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Components/CapsuleComponent.h"
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

UGA_RollerDrone::UGA_RollerDrone()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
}

void UGA_RollerDrone::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (Enemy)
	{
		StunHandle = Enemy->OnTaserStatusChanged.AddLambda([this](bool bTased)
			{
				StunChanged(bTased);
			});
	}

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

	RollerSkeletalMesh->SetVisibility(false);

	if (K2_HasAuthority())
	{
		Enemy->Multicast_SetRollerVisibility(false, true, 0.0f);
	}

	UAbilityTask_WaitGameplayEvent* WaitForRollUpActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRollUpTag());
	WaitForRollUpActivation->EventReceived.AddDynamic(this, &UGA_RollerDrone::RollUp);
	WaitForRollUpActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForUnRollActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUnrollTag());
	WaitForUnRollActivation->EventReceived.AddDynamic(this, &UGA_RollerDrone::UnRoll);
	WaitForUnRollActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForHideRollerActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetHideRollerTag());
	WaitForHideRollerActivation->EventReceived.AddDynamic(this, &UGA_RollerDrone::HideRoller);
	WaitForHideRollerActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForShowRollerActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetShowRollerTag());
	WaitForShowRollerActivation->EventReceived.AddDynamic(this, &UGA_RollerDrone::ShowRoller);
	WaitForShowRollerActivation->ReadyForActivation();
}

void UGA_RollerDrone::RollUp(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Enemy->Multicast_SetRollerVisibility(true, false, 1.0f);
	}
}

void UGA_RollerDrone::RollUpFrameDelay()
{
}

void UGA_RollerDrone::UnRoll(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Enemy->Multicast_SetRollerVisibility(false, true, 0.0f);
	}
}

void UGA_RollerDrone::UnRollFrameDelay()
{
}

void UGA_RollerDrone::HideRoller(FGameplayEventData Payload)
{
	Enemy->GetMesh()->SetVisibility(true);
	RollerSkeletalMesh->SetVisibility(false);
}

void UGA_RollerDrone::ShowRoller(FGameplayEventData Payload)
{
	Enemy->GetMesh()->SetVisibility(false);
	RollerSkeletalMesh->SetVisibility(true);
}

void UGA_RollerDrone::StunChanged(bool bTased)
{
	UE_LOG(LogTemp, Error, TEXT("Changed stun changed tag"));
	bStunned = bTased;
}

void UGA_RollerDrone::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (StunHandle.IsValid() && Enemy)
	{
		StopDurationAudioEffect();
		Enemy->OnTaserStatusChanged.Remove(StunHandle);
	}
}
