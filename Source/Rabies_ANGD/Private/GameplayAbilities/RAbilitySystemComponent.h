// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/RAbilitySystemTypes.h"
#include "RAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class URAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, WithValidation) //this will singnal the server to call this function. Realiable means it will eventually called. UnRealiable means it could be dropped.
	void ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectToApply, int Level = 1);

	void ApplyInitialEffects();
	void GrantInitialAbilities();
	void ApplyFullStat();

	TArray<const FGameplayAbilitySpec*> GetGrantedNonGenericAbilities() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Init")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Init")
	TSubclassOf<UGameplayEffect> FullStatEffect;

	// Include Generic Abilities and Regular Abilities
};
