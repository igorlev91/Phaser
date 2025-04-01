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

private:
	FTimerHandle ComboTimerHandle;

	class UBehaviorTreeComponent* CachedOwnerComp;

	APawn* OwningPawn;

	UFUNCTION()
	void DoAttack();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Attack Count")
	int punchCount;
};
