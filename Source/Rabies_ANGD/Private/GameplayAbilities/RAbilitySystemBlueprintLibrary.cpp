// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Framework/CooldownMagnitudeCalculation.h"

float URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(const UGameplayAbility* AbilityCDO, UAbilitySystemComponent* OwnerASC)
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
