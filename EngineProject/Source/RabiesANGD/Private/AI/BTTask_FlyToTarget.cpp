// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FlyToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_FlyToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector ForwardDirection = Pawn->GetActorForwardVector();

	// Calculate launch speed based on distance
	const float BaseSpeed = 600.0f; // Minimum launch speed
	const float ScaleFactor = 0.1f; // Adjust this to control the effect of distance

	float ZVariation = FMath::RandRange(-800.0f, 200.0f); // Random upward/downward force
	FVector LaunchVelocity = ForwardDirection * (BaseSpeed + ScaleFactor * dis);
	LaunchVelocity.Z += ZVariation;


	// Launch the enemy (preserve existing Z movement if needed)
	Pawn->LaunchCharacter(LaunchVelocity, true, false);

	return EBTNodeResult::Succeeded;
}
