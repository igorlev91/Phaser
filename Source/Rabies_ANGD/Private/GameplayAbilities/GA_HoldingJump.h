// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_HoldingJump.generated.h"

/**
 * 
 */
UCLASS()
class UGA_HoldingJump : public UGA_AbilityBase
{

public:
	UGA_HoldingJump();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void StopHoldingJump(FGameplayEventData Payload);
};
