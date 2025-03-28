// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffectExecutionCalculation.h"

#include "GameplayModMagnitudeCalculation.h"
#include "RDotSpecialCooldownCalculation.generated.h"

/**
 * 
 */
UCLASS()
class URDotSpecialCooldownCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	// Override this function to define custom calculation
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
