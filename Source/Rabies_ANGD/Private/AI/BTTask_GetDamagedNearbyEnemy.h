// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetDamagedNearbyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_GetDamagedNearbyEnemy : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
