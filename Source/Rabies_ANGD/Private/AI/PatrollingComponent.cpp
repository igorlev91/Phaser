// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PatrollingComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPatrollingComponent::UPatrollingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UPatrollingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> patrolPoints;
	TArray<ATargetPoint*> newPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), patrolPoints);
	for (AActor* point : patrolPoints)
	{
		newPoints.Add(Cast<ATargetPoint>(point));
	}
	PatrolPoints = newPoints;
}

const ATargetPoint* UPatrollingComponent::GetNextPatrolPoint()
{
	ATargetPoint* NextPatrolPoint = PatrolPoints[NextPatrolPointIndex];
	NextPatrolPointIndex = (NextPatrolPointIndex + 1) % PatrolPoints.Num();

	return NextPatrolPoint;
}