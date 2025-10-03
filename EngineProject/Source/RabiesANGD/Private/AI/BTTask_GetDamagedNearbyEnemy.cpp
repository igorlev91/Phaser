// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetDamagedNearbyEnemy.h"
#include "AI/PatrollingComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TargetPoint.h"
#include "Framework/EOSActionGameState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"
#include "Framework/EOSPlayerState.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameStateBase.h"
#include "AI/REnemyAIController.h"
#include "Enemy/REnemyBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_GetDamagedNearbyEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Pawn = AIC->GetCharacter();
	AREnemyBase* ThisEnemy = Cast<AREnemyBase>(Pawn);

	if (ThisEnemy == nullptr)
		return EBTNodeResult::Failed;

	UAIPerceptionComponent* PerceptionComponent = AIC->GetPerceptionComponent();
	if (!PerceptionComponent)
		return EBTNodeResult::Failed;

	//
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AREnemyBase::StaticClass(), AllActors);
	TArray<AREnemyBase*> eligableTargets;

	for (AActor* Actor : AllActors)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(Actor);
		if (enemy == nullptr)
			continue;

		if (enemy == ThisEnemy)
			continue;

		if (enemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
			continue;

		if (enemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullHealthTag()))
			continue;

		eligableTargets.Add(enemy);
	}

	float closestDistance = 500000.0f;
	AREnemyBase* resultingTarget = nullptr;

	for (AREnemyBase* target : eligableTargets)
	{
		if (target == nullptr)
			continue;

		float DistanceSquared = FVector::Dist(ThisEnemy->GetActorLocation(), target->GetActorLocation());
		if (DistanceSquared < closestDistance)
		{
			closestDistance = DistanceSquared;
			resultingTarget = target;
		}
	}

	if (resultingTarget == nullptr)
		return EBTNodeResult::Failed;

	AIC->GetBlackboardComponent()->SetValueAsObject(TargetBlackboardKeyName, resultingTarget);

	FAIStimulus Stimulus;
	Stimulus.Tag = FName("Sight"); // Set the tag for the type of stimulus
	Stimulus.Strength = 1.0f; // Strength of the stimulus (e.g., full strength)
	Stimulus.StimulusLocation = resultingTarget->GetActorLocation(); // Where the stimulus is coming from
	Stimulus.WasSuccessfullySensed(); // Whether it was successfully sensed
	Cast<AREnemyAIController>(AIC)->bAutoSense = true;
	Cast<AREnemyAIController>(AIC)->TargetPerceptionUpdated(resultingTarget, Stimulus);

	return EBTNodeResult::Succeeded;
}
