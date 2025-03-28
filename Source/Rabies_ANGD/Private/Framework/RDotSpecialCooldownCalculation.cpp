// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RDotSpecialCooldownCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float URDotSpecialCooldownCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float BaseMagnitude = 7.0f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float specialCooldown = SourceASC->GetNumericAttributeBase(URAttributeSet::GetAbilityCooldownReductionAttribute()); // THIS IS FOR THE SPECIAL

        BaseMagnitude *= specialCooldown;
        BaseMagnitude = FMath::Max(BaseMagnitude, 0.1f); // Ensure the result is not 0
    }

    return BaseMagnitude;
}
