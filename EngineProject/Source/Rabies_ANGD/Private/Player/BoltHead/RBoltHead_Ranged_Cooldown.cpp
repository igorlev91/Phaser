// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/RBoltHead_Ranged_Cooldown.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float URBoltHead_Ranged_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float baseSpeed = 1.2f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute());

        baseSpeed *= modifiedSpeed;

        modifiedSpeed = FMath::Max(baseSpeed, 0.1f); // Ensure the result is not 0
    }

    return baseSpeed;
}
