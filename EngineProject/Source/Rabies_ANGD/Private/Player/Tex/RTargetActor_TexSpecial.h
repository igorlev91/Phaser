// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RTargetActor_TexSpecial.generated.h"

/**
 * 
 */
UCLASS()
class ARTargetActor_TexSpecial : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ARTargetActor_TexSpecial();

	void SetTargettingRadus(float targettingRadus);

	void SetOwningPlayerControler(class ARPlayerController* myController, float yawAdjustment);

	void Tick(float DeltaSecond) override;

	virtual bool IsConfirmTargetingAllowed() override;
	virtual void ConfirmTargetingAndContinue() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Targetting")
	class USphereComponent* TargetSphere;

	float YawAdjustment;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* CylinderMesh;

	UFUNCTION()
	void SetMeshPosition(FHitResult givenPlayerView);


	class ARPlayerController* MyPlayerController;

	FHitResult GetPlayerView(float yawAdjustment) const;
};
