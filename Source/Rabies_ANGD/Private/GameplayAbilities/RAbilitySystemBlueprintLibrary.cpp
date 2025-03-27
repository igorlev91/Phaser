// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Framework/RGattlingAttackSpeedCalculation.h"

float URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO, UAbilitySystemComponent* OwnerASC)
{
	float duration = 0;

	UGameplayEffect* cooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (cooldownEffect)
	{
		if (cooldownEffect->DurationMagnitude.GetCustomMagnitudeCalculationClass())
		{
			const TSubclassOf<UGameplayModMagnitudeCalculation> customClass = cooldownEffect->DurationMagnitude.GetCustomMagnitudeCalculationClass();
			if (customClass)
			{
				URGattlingAttackSpeedCalculation* customModCalc = NewObject<URGattlingAttackSpeedCalculation>(customClass);
				if (customModCalc)
				{
					FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
					EffectContext.AddInstigator(OwnerASC->GetAvatarActor(), OwnerASC->GetAvatarActor());

					FGameplayEffectSpec spec(cooldownEffect, EffectContext, 1.0f);
					duration = customModCalc->CalculateBaseMagnitude(spec);
				}
			}
		}
		else
		{
			cooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, duration);
		}
	}
	return duration;
}
