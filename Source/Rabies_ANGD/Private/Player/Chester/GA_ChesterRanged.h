// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ChesterRanged.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ChesterRanged : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ChesterRanged();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void Fire(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* ShootingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> RangedDamage;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class ARChester_RangeProj> ChesterProjectile;

	UPROPERTY()
	class ARPlayerBase* Player;
};
