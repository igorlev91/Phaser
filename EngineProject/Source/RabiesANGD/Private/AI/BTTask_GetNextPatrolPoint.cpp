// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetNextPatrolPoint.h"

#include "AI/PatrollingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Pawn = AIC->GetCharacter();

	UPatrollingComponent* PatrolingComp = Pawn->GetComponentByClass<UPatrollingComponent>();
	if (PatrolingComp)
	{
		const ATargetPoint* NextPoint = PatrolingComp->GetNextPatrolPoint();
		if (NextPoint)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, NextPoint->GetActorLocation());
			return EBTNodeResult::Succeeded;
		}
	}


	return EBTNodeResult::Failed;
}
