// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrollingComponent.generated.h"


class ATargetPoint;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPatrollingComponent();

	const ATargetPoint* GetNextPatrolPoint();

private:
	UPROPERTY(EditAnywhere, Category = "Patrolling")
	TArray<ATargetPoint*> PatrolPoints;

	void RandomizeArray(TArray<ATargetPoint*>& Array);

	int NextPatrolPointIndex = 0;

protected:

	virtual void BeginPlay() override;
		
};
