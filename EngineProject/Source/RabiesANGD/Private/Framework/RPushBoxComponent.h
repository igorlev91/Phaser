// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RPushBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class URPushBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	void StartDetection();
	void DoAttackCheck();
	void EndDetection();

private:
	TSet<const AActor*> AlreadyDetectedActors;

	void TargetFound(AActor* OverlappedActor);
};
