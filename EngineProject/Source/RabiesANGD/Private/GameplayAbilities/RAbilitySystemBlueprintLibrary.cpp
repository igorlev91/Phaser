// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Framework/CooldownMagnitudeCalculation.h"

float URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* OwnerASC)
{
	float duration = 0;

	UGameplayEffect* cooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (cooldownEffect)
	{
		if (cooldownEffect->DurationMagnitude.GetCustomMagnitudeCalculationClass())
		{
			const UGA_AbilityBase* thisAbility = Cast<UGA_AbilityBase>(AbilityCDO);
			UGameplayModMagnitudeCalculation* customModCalc = NewObject<UGameplayModMagnitudeCalculation>(GetTransientPackage(), thisAbility->CooldownCalculationClass);
			if (customModCalc)
			{
				FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
				EffectContext.AddInstigator(OwnerASC->GetAvatarActor(), OwnerASC->GetAvatarActor());

				FGameplayEffectSpec spec(cooldownEffect, EffectContext, 1.0f);
				duration = customModCalc->CalculateBaseMagnitude(spec);
			}

			/*const TSubclassOf<UGameplayModMagnitudeCalculation> customClass = cooldownEffect->DurationMagnitude.GetCustomMagnitudeCalculationClass();
			if (customClass)
			{
			}*/
		}
		else
		{
			cooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, duration);
		}
	}
	return duration;
}

float URAbilitySystemBlueprintLibrary::GetAbilityCooldownDuration(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* OwnerASC)
{
	return GetEffectTimeDuration(AbilityCDO->GetCooldownGameplayEffect(), OwnerASC);
}

float URAbilitySystemBlueprintLibrary::GetAbilityCooldownRemaining(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* OwnerASC)
{
	return GetEffectTimeRemaining(AbilityCDO->GetCooldownGameplayEffect(), OwnerASC);
}

float URAbilitySystemBlueprintLibrary::GetEffectTimeRemaining(const TSubclassOf<UGameplayEffect>& EffectClass, const UAbilitySystemComponent* OwnerASC)
{
	FGameplayEffectQuery Query;
	Query.EffectDefinition = EffectClass;

	float MaxTimeRemaining = 0.f;

	for (auto& RemainingTime : OwnerASC->GetActiveEffectsTimeRemaining(Query))
	{
		if (RemainingTime > MaxTimeRemaining)
		{
			MaxTimeRemaining = RemainingTime;
		}
	}

	return MaxTimeRemaining;
}

float URAbilitySystemBlueprintLibrary::GetEffectTimeRemaining(const UGameplayEffect* EffectCDO, const UAbilitySystemComponent* OwnerASC)
{
	return GetEffectTimeRemaining(EffectCDO->GetClass(), OwnerASC);
}

float URAbilitySystemBlueprintLibrary::GetEffectTimeDuration(const TSubclassOf<UGameplayEffect>& EffectClass, const UAbilitySystemComponent* OwnerASC)
{
	FGameplayEffectQuery Query;
	Query.EffectDefinition = EffectClass;
	
	float MaxTimeDuration = 0.f;

	for (auto& TimeDuration : OwnerASC->GetActiveEffectsDuration(Query))
	{
		if (TimeDuration > MaxTimeDuration)
		{
			MaxTimeDuration = TimeDuration;
		}
	}

	return MaxTimeDuration;
}

float URAbilitySystemBlueprintLibrary::GetEffectTimeDuration(const UGameplayEffect* EffectCDO, const UAbilitySystemComponent* OwnerASC)
{
	return GetEffectTimeDuration(EffectCDO->GetClass(), OwnerASC);
}
