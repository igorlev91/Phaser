// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_EnemyMissile.generated.h"

/**
 * 
 */
UCLASS()
class UGA_EnemyMissile : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_EnemyMissile();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* AttackingAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class AREnemyMissileProj> MissileProjectile;

	FActiveGameplayEffectHandle SlowDownEffectHandle;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY()
	class AREnemyBase* Enemy;

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UPROPERTY()
	class AAIController* EnemyAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY()
	AActor* TargetActor;
};
