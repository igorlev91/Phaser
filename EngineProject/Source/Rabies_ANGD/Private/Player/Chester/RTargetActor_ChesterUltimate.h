// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RTargetActor_ChesterUltimate.generated.h"

/**
 * 
 */
UCLASS()
class ARTargetActor_ChesterUltimate : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ARTargetActor_ChesterUltimate();

	void SetTargettingRadus(float targettingRadus);

	void SetTargettingRange(float newTargettingRange);

	void SetOwningPlayerControler(class ARPlayerController* myController);

	void Tick(float DeltaSecond) override;

	virtual bool IsConfirmTargetingAllowed() override;
	virtual void ConfirmTargetingAndContinue() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Targetting")
	class USphereComponent* TargetSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* CylinderMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* SphereMesh;

	float TargettingRange;

	class ARPlayerController* MyPlayerController;

	FHitResult GetPlayerView() const;
};
