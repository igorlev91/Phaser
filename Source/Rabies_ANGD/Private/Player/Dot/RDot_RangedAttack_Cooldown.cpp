// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Dot/RDot_RangedAttack_Cooldown.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Player/RPlayerBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

float URDot_RangedAttack_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    float baseSpeed = 0.2f;

    const UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

    // Check if source and target exist
    if (SourceASC)
    {
        // Get the instigator’s strength attribute
        float modifiedSpeed = SourceASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute());


        baseSpeed *= modifiedSpeed;

        modifiedSpeed = FMath::Max(baseSpeed, 0.01f); // Ensure the result is not 0
    }

    return baseSpeed;
}
