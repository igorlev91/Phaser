// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttributeSet.h"
#include "RBoltHead_Ranged_Cooldown.generated.h"

/**
 * 
 */
UCLASS()
class URBoltHead_Ranged_Cooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	

public:
	// Override this function to define custom calculation
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
