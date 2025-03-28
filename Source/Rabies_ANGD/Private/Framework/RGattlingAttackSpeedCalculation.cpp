// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RGattlingAttackSpeedCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float URGattlingAttackSpeedCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float BaseMagnitude = 0.25f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float rangedAttackSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute()); // THIS IS FOR THE RANGEDATTACK

        BaseMagnitude *= rangedAttackSpeed;
        BaseMagnitude = FMath::Max(BaseMagnitude, 0.1f); // Ensure the result is not 0
    }

    return BaseMagnitude;
}
