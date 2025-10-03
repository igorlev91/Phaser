// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttributeSet.h"
#include "CooldownMagnitudeCalculation.generated.h"

/**
 * 
 */
UCLASS()
class UCooldownMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	// Override this function to define custom calculation
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	float BaseShootingSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	bool rangedCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	bool meleeCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	bool specialCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	bool ultimateCooldown;

};
