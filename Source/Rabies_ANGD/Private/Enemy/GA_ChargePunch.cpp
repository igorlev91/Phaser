// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_ChargePunch.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"


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

UGA_ChargePunch::UGA_ChargePunch()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
}

void UGA_ChargePunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_ChargePunch::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetExpTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ChargePunch::HandleDamage);
	WaitForDamage->ReadyForActivation();
}

void UGA_ChargePunch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UGA_ChargePunch::TryCommitAttack(FGameplayEventData Payload)
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

			playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChargePunchMontage);
			playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ChargePunch::StopChargePunch);
			playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ChargePunch::StopChargePunch);
			playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ChargePunch::StopChargePunch);
			playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ChargePunch::StopChargePunch);
			playTargettingMontageTask->ReadyForActivation();

			GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &UGA_ChargePunch::PunchForward, 0.85f, false);
			GetWorld()->GetTimerManager().SetTimer(PopupTimerHandle, this, &UGA_ChargePunch::Popup, 0.7f, false);
		}
	}
}

void UGA_ChargePunch::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(PunchDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_ChargePunch::StopChargePunch()
{
	GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);

}

void UGA_ChargePunch::PunchForward()
{
	FVector EnemyLocation = Enemy->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector Direction = (TargetLocation - EnemyLocation).GetSafeNormal();

	float DistanceToTarget = FVector::Dist(EnemyLocation, TargetLocation);

	const float BaseSpeed = 900.0f;     // Minimum launch speed.
	const float ScaleFactor = 0.9f;     // How much speed increases with distance.
	float LaunchSpeed = BaseSpeed + ScaleFactor * DistanceToTarget;

	FVector LaunchVelocity = Direction * LaunchSpeed;

	Enemy->LaunchCharacter(LaunchVelocity, true, true);
}

void UGA_ChargePunch::Popup()
{
	FVector LaunchVelocity = FVector(0.0f, 0.0f, 100.0f); // Only Z movement
	Enemy->LaunchCharacter(LaunchVelocity, true, false);
}
