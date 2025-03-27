// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RHealthReductionCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float URHealthReductionCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float BaseMagnitude = 5.0f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = Cast<ARPlayerBase>(Spec.GetContext().GetEffectCauser())->GetAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC && TargetASC)
    {
        // Get the instigator’s strength attribute
        float Strength = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackStrengthAttribute()); // THIS IS FOR THE RANGEDATTACK

        // Get the target’s defense attribute
        float Defense = TargetASC->GetNumericAttributeBase(URAttributeSet::GetDamageReductionAttribute());

        // Calculate the magnitude based on strength and defense
        BaseMagnitude = Strength - Defense;
        BaseMagnitude = FMath::Max(BaseMagnitude, 0.0f); // Ensure the result deals at least 1 damage
    }

    return BaseMagnitude;
}
