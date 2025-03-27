// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "RAbilitySystemBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class URAbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	static float GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO, class UAbilitySystemComponent* OwnerASC);
};
