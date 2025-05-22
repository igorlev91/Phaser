// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_EnemyHeal.generated.h"

/**
 * 
 */
UCLASS()
class UGA_EnemyHeal : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_EnemyHeal();

private:

	UPROPERTY()
	class AREnemyBase* Enemy;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Heal;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* HealingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HealingParticle;

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	class AAIController* EnemyAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";
};
