// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "RBoltHead_Ultimate_Cooldown.generated.h"

/**
 * 
 */
UCLASS()
class URBoltHead_Ultimate_Cooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	// Override this function to define custom calculation
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
