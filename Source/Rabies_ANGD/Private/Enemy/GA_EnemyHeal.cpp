// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_EnemyHeal.h"

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

UGA_EnemyHeal::UGA_EnemyHeal()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
}

void UGA_EnemyHeal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_EnemyHeal::TryCommitAttack);
	WaitForActivation->ReadyForActivation();
}

void UGA_EnemyHeal::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UGA_EnemyHeal::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (Enemy)
		{

			UObject* target = EnemyAI->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
			TargetActor = Cast<AActor>(target);
			AREnemyBase* TargetEnemy = Cast<AREnemyBase>(TargetActor);

			if (TargetEnemy == nullptr)
				return;

			if (TargetEnemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
				return;

			if (TargetEnemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullHealthTag()))
				return;

			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState)
			{
				gameState->Multicast_ShootTexUltimate(HealingParticle, Enemy, Enemy->GetActorLocation(), TargetEnemy->GetActorLocation(), TargetEnemy->GetActorLocation());
			}

			FGameplayEffectSpecHandle specHandle = Enemy->GetAbilitySystemComponent()->MakeOutgoingSpec(Heal, 1.0f, Enemy->GetAbilitySystemComponent()->MakeEffectContext());

			FGameplayEffectSpec* spec = specHandle.Data.Get();
			if (spec)
			{

				TargetEnemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
			}
		}

	}
}
