// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_CompareTargetDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_CompareTargetDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Pawn = AIC->GetCharacter();

	UObject* target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
	AActor* targetActor = Cast<AActor>(target);

	if (targetActor == nullptr || Pawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	float dis = FVector::Dist(Pawn->GetActorLocation(), targetActor->GetActorLocation());
	//UE_LOG(LogTemp, Error, TEXT("Range %f"), dis);

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceBlackboardKeyName, dis);

	return EBTNodeResult::Succeeded;
}
