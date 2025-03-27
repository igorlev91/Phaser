// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_Revive.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Revive : public UGA_AbilityBase
{
public:
	UGA_Revive();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	float CurrentHoldDuration;

	FTimerHandle ReviveHandle;

	void Hold(float timeRemaining);

	UPROPERTY(EditDefaultsOnly, Category = "Revive")
	TSubclassOf<class UGameplayEffect> ReviveEffectClass;

	UPROPERTY()
	class ARPlayerBase* Player;

	UFUNCTION()
	void StopHoldingRevive(FGameplayEventData Payload);
};
