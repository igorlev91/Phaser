// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_TexPassive.generated.h"

/**
 * 
 */
UCLASS()
class UGA_TexPassive : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_TexPassive();

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Bonus Stats")
	TSubclassOf<class UGameplayEffect> BonusStatsClass;

	UFUNCTION()
	void FinishStealth(FGameplayEventData Payload);

	FActiveGameplayEffectHandle StatsEffectHandle;

	UPROPERTY()
	class ARPlayerBase* Player;
};
