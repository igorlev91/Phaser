// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"

void URAbilitySystemComponent::ApplyGameplayEffect_Implementation(TSubclassOf<UGameplayEffect> EffectToApply, int Level)
{
	FGameplayEffectSpecHandle specHandle = MakeOutgoingSpec(EffectToApply, Level, MakeEffectContext());
	ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
}

bool URAbilitySystemComponent::ApplyGameplayEffect_Validate(TSubclassOf<UGameplayEffect> EffectToApply, int Level)
{
	return true;
}

void URAbilitySystemComponent::ApplyInitialEffects()
{
	for (const TSubclassOf<UGameplayEffect>& Effect : InitialEffects)
	{
		ApplyGameplayEffect(Effect, 1);
	}
}

void URAbilitySystemComponent::GrantInitialAbilities()
{
	for (const TPair<EAbilityInputID, TSubclassOf<UGA_AbilityBase>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec{ AbilityPair.Value, 1, (int)AbilityPair.Key, GetOwner() });
	}
}

void URAbilitySystemComponent::ApplyFullStat()
{
	if (FullStatEffect)
		ApplyGameplayEffect(FullStatEffect);
}

TArray<const FGameplayAbilitySpec*> URAbilitySystemComponent::GetGrantedNonGenericAbilities() const
{
	TArray<const FGameplayAbilitySpec*> GrantedAbilitySpecs;

	for (const TPair<EAbilityInputID, TSubclassOf<UGA_AbilityBase>>& AbilityPair : Abilities)
	{
		const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityPair.Value);
		GrantedAbilitySpecs.Add(AbilitySpec);
	}

	return GrantedAbilitySpecs;
}