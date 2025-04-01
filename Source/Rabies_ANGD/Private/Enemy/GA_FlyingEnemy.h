// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_FlyingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UGA_FlyingEnemy : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_FlyingEnemy();

private:

	FActiveGameplayEffectHandle GravityFallEffectHandle;

	UPROPERTY()
	float FlyingHeight;

	UFUNCTION()
	void EndFlying(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* DeathSmoke;

	UPROPERTY()
	class AREnemyBase* Enemy;

	FTimerHandle FlyingHandle;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityFallClass;

	FDelegateHandle StunHandle;

	UPROPERTY()
	bool bStunned;

	UFUNCTION()
	void StunChanged(bool bTased);

	UFUNCTION()
	void Fly();
};
