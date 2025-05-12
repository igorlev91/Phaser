// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetRandomPointAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class UBTTask_GetRandomPointAtTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName RandomBlackboardKeyName = "RandomPoint";
};
