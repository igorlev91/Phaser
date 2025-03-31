// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RTargetActor_DotUltimate.generated.h"

/**
 * 
 */
UCLASS()
class ARTargetActor_DotUltimate : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ARTargetActor_DotUltimate();

	UFUNCTION(Server, Reliable)
	void Server_SetCylinderSize(FVector scale);

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* CylinderMesh;
};
