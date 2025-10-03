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
	static float GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO, const class UAbilitySystemComponent* OwnerASC);
	static float GetAbilityCooldownDuration(const UGameplayAbility* AbilityCDO, const class UAbilitySystemComponent* OwnerASC);
	static float GetAbilityCooldownRemaining(const UGameplayAbility* AbilityCDO, const class UAbilitySystemComponent* OwnerASC);

	static float GetEffectTimeDuration(const TSubclassOf<UGameplayEffect>& EffectClass, const class UAbilitySystemComponent* OwnerASC);
	static float GetEffectTimeDuration(const UGameplayEffect* EffectCDO, const class UAbilitySystemComponent* OwnerASC);
	static float GetEffectTimeRemaining(const TSubclassOf<UGameplayEffect>& EffectClass, const class UAbilitySystemComponent* OwnerASC);
	static float GetEffectTimeRemaining(const UGameplayEffect* EffectCDO, const class UAbilitySystemComponent* OwnerASC);
};
