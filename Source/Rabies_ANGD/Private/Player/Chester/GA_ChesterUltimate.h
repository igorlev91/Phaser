// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ChesterUltimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ChesterUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ChesterUltimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_DotSpecial> targetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class ARChester_UltimateProj> ChesterProjectile;

	FActiveGameplayEffectHandle SlowDownEffectHandle;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> SlowdownClass;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;
};
