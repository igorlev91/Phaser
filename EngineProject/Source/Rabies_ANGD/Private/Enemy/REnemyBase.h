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

	UFUNCTION()
	void SetCollisionStatus(bool status);

	void RecievedDamage(float damage);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int GibletCount = 3;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetRollerVisibility(bool bShowRoller, bool bShowMain, float blendShape);

	UFUNCTION()
	void SetRollerVisibilityDelay(bool bShowRoller, bool bShowMain, USkeletalMeshComponent* roller);

private:

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* DeathSound;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf <class ADamagePopupActor> DamagePopupActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bIsDeadlock;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bIsDeadlockComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float deathTimer = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float itemDropChance = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float itemsToDrop = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class UGameplayEffect> GravityFallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* DeadlockSmokeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* DeadlockExplosionEffect;

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
	UFUNCTION()
	void DropItemServerDeathRequest();

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateEnemyDeath();

};
