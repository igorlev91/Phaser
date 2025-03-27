// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PatrollingComponent.h"
#include "Engine/TargetPoint.h"

// Sets default values for this component's properties
UPatrollingComponent::UPatrollingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}

const ATargetPoint* UPatrollingComponent::GetNextPatrolPoint()
{
	ATargetPoint* NextPatrolPoint = PatrolPoints[NextPatrolPointIndex];
	NextPatrolPointIndex = (NextPatrolPointIndex + 1) % PatrolPoints.Num();

	return NextPatrolPoint;
}
