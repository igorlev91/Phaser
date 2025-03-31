// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotUltimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotUltimate();

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

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontageAir;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontageAir;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;
};
