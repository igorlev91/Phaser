// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CooldownMagnitudeCalculation.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float UCooldownMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float baseSpeed = 1.0f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute());

        if(meleeCooldown)
            modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetMeleeAttackCooldownReductionAttribute());

        if(rangedCooldown)
            modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute());

        if (specialCooldown)
            modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetAbilityCooldownReductionAttribute());

        if (ultimateCooldown)
            modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetUltimateCooldownReductionAttribute());

        baseSpeed *= modifiedSpeed;
        UE_LOG(LogTemp, Warning, TEXT("%f Base Magnitude "), baseSpeed);

        modifiedSpeed = FMath::Max(baseSpeed, 0.1f); // Ensure the result is not 0
    }

    return baseSpeed;
}
