// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_BrawlerCombo.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_BrawlerCombo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	float Threshold = 3.0f;

	float TimeRemaining = 3.0f;

	class UBehaviorTreeComponent* CachedOwnerComp;

	APawn* OwningPawn;
};
