// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_RangedGattlingAttack.generated.h"

/**
 * 
 */
UCLASS()
class UGA_RangedGattlingAttack : public UGA_AbilityBase
{
public:
	UGA_RangedGattlingAttack();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UFUNCTION()
	void Fire(FGameplayEventData Payload);

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UFUNCTION()
	void AbilityInputPressed(float TimeWaited);

	UFUNCTION()
	void SetupWaitInputTask();

	const FGameplayAbilityActorInfo* actorInfo;

	UPROPERTY()
	FGameplayAbilityActivationInfo activationInfo;

	UPROPERTY()
	FGameplayAbilitySpecHandle cooldownHandle;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RangedGattlingDamage;

	FDelegateHandle ClientHitScanHandle;

	UFUNCTION()
	void StopAttacking(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	FTimerHandle RangedAttackHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<class UGameplayModMagnitudeCalculation> ShootingCooldownClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<class UGameplayEffect> ShootingCooldownGameplayEffect;
};
