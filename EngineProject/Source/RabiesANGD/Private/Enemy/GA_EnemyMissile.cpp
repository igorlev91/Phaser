// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_EnemyMissile.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Enemy/REnemyMissileProj.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "AI/REnemyAIController.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/Chester/RChester_UltimateProj.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"
#include "Targeting/RTargetActor_DotSpecial.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_EnemyMissile::UGA_EnemyMissile()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
}

void UGA_EnemyMissile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_EnemyMissile::TryCommitAttack);
	WaitForActivation->ReadyForActivation();
}

void UGA_EnemyMissile::SendOffAttack(FGameplayEventData Payload)
{
}

void UGA_EnemyMissile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UGA_EnemyMissile::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Enemy->ServerPlayAnimMontage(AttackingAnim);

		UObject* target = EnemyAI->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
		TargetActor = Cast<AActor>(target);

		FVector EnemyForwardVector = Enemy->GetActorForwardVector();
		FVector EnemyLocation = Enemy->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();

		FVector viewLoc = EnemyLocation + EnemyForwardVector * 200.0f;
		FVector viewRot = TargetLocation - viewLoc;
		viewRot.Normalize();

		FRotator RotationToTarget = viewRot.Rotation();

		AREnemyMissileProj* newProjectile = GetWorld()->SpawnActor<AREnemyMissileProj>(MissileProjectile, viewLoc, RotationToTarget);
		newProjectile->Init(AttackDamages, Enemy);
		newProjectile->InitOwningCharacter(Enemy);
		newProjectile->SetOwner(Enemy);
		StartDurationAudioEffect();
	}
}
