// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BrawlerCombo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_BrawlerCombo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (TimeRemaining <= 0)
	{
		TimeRemaining = 3.0f;
		return EBTNodeResult::Succeeded;
	}

	CachedOwnerComp = &OwnerComp;
	if (CachedOwnerComp == nullptr)
		return EBTNodeResult::Failed;

	OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn == nullptr)
		return EBTNodeResult::Failed;

	TimeRemaining = 3.0f;
	Threshold = 3.0f;

	bNotifyTick = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_BrawlerCombo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TimeRemaining -= DeltaSeconds;
	if (TimeRemaining < Threshold)
	{
		Threshold -= 0.6f;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningPawn, URAbilityGenericTags::GetBasicAttackActivationTag(), FGameplayEventData());
	}

	if (TimeRemaining <= 0)
	{
		TimeRemaining = 3.0f;
		ExecuteTask(OwnerComp, NodeMemory);
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}