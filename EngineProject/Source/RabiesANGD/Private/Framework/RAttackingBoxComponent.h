// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "RAttackingBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class URAttackingBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	void StartDetection();
	void DoAttackCheck();
	void EndDetection();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bEnemy;

private:
	TSet<const AActor*> AlreadyDetectedActors;

	void TargetFound(AActor* OverlappedActor);
};
