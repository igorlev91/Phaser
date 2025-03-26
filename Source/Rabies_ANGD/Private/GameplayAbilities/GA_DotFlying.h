// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotFlying.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotFlying : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotFlying();

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> FlyingSpeedClass;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TakeOffMontage;

	UFUNCTION()
	void StopFlying(FGameplayEventData Payload);

	UFUNCTION()
	void StopTakeOff(FGameplayEventData Payload);

	FTimerHandle TakeOffHandle;

	float CurrentHoldDuration;

	void Hold(float timeRemaining);

	UPROPERTY()
	class ARPlayerBase* Player;
};
