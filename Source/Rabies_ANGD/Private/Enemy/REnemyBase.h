// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RCharacterBase.h"
#include "REnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class AREnemyBase : public ARCharacterBase
{
	GENERATED_BODY()
	
public:
	AREnemyBase();

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateAimingTagChange(bool state);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void InitLevel(int level);

private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<UGameplayEffect> WaveLevelUpgrade;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float HealthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MeleeStrengthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RangedStrengthOnLevelUp = 1;


	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComp;

	FTimerHandle LevelHandle;

	UFUNCTION(NetMulticast, Unreliable)
	void CommitLevel();

	UFUNCTION()
	void HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);

};
