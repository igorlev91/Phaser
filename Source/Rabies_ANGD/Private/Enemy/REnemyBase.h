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

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UTexture* EnemyIcon;

private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bIsDeadlock;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bIsDeadlockComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float deathTimer = 1.2f;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class UGameplayEffect> GravityFallClass;

	FTimerHandle LevelHandle;
	FTimerHandle DeathHandle;

	UFUNCTION(NetMulticast, Unreliable)
	void CommitLevel();

	UFUNCTION()
	void HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);
	
	UFUNCTION()
	void DelayServerDeathRequest();

public:
	UFUNCTION(NetMulticast, Unreliable)
	void UpdateEnemyDeath();

};
