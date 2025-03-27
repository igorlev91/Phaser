// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_PlayAnimMontage.generated.h"

/**
 * 
 */

struct PlayMontageNodeData
{
	UAnimMontage* MontagePlayed;
	float MontageTimeLeft;
};

UCLASS()
class UBTTask_PlayAnimMontage : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAnimMontage();
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetSpecialMemorySize() const override;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* MontageToPlay;
};
