// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "REnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class AREnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AREnemyAIController();

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void BeginPlay() override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus);

	bool bAutoSense;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	

private:

	UFUNCTION()
	void PlayerDeadStatusUpdated(bool bIsDead);

	UPROPERTY()
	class AREnemyBase* Enemy;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName AIVisionSocketName = "head";

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAISenseConfig_Touch* TouchConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UFUNCTION()
	void TargetForgotton(AActor* Target);

	UFUNCTION()
	void ProcessForgottonTarget(AActor* Target);

	void PawnDeathStatusChanged(bool bIsDead);
};
