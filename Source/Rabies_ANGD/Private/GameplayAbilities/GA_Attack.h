// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_Attack.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Attack : public UGA_AbilityBase
{

public:
	UGA_Attack();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageTest;

	UFUNCTION()
	void AbilityInputPressed(float TimeWaited);

	bool bAttackCommitted;

	void SetupWaitInputTask();

	UFUNCTION()
	void StopAttacking(FGameplayEventData Payload);
};
