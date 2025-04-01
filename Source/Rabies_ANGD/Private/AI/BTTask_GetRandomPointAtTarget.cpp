// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetRandomPointAtTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_GetRandomPointAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Pawn = AIC->GetCharacter();

	UObject* target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
	AActor* targetActor = Cast<AActor>(target);
	
	if (targetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	float RandomDistance = FMath::RandRange(50.0f, 250.0f);
	float RandomAngle = FMath::RandRange(0.0f, (2 * 3.14f));
	FVector CircleOffset = FVector(FMath::Cos(RandomAngle) * RandomDistance, FMath::Sin(RandomAngle) * RandomDistance, 0.0f);
	FVector RandomLocation = targetActor->GetActorLocation() + CircleOffset;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		FNavLocation pojectedLoc;
		if (NavSys->ProjectPointToNavigation(RandomLocation, pojectedLoc))
		{
			RandomLocation = pojectedLoc.Location;
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(RandomBlackboardKeyName, RandomLocation);

	return EBTNodeResult::Succeeded;
}
