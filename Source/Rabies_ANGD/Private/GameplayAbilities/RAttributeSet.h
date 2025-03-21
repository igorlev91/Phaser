// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "RAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class URAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ATTRIBUTE_ACCESSORS(URAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(URAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(URAttributeSet, MeleeAttackStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, RangedAttackStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, AbilityStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, UltimateStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, MeleeAttackCooldownReduction)
	ATTRIBUTE_ACCESSORS(URAttributeSet, RangedAttackCooldownReduction)
	ATTRIBUTE_ACCESSORS(URAttributeSet, AbilityCooldownReduction)
	ATTRIBUTE_ACCESSORS(URAttributeSet, UltimateCooldownReduction)
	ATTRIBUTE_ACCESSORS(URAttributeSet, MovementSpeed)
	ATTRIBUTE_ACCESSORS(URAttributeSet, ReviveSpeed)
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_MeleeAttackStrength)
	FGameplayAttributeData MeleeAttackStrength;

	UPROPERTY(ReplicatedUsing = OnRep_RangedAttackStrength)
	FGameplayAttributeData RangedAttackStrength;

	UPROPERTY(ReplicatedUsing = OnRep_AbilityStrength)
	FGameplayAttributeData AbilityStrength;

	UPROPERTY(ReplicatedUsing = OnRep_UltimateStrength)
	FGameplayAttributeData UltimateStrength;

	UPROPERTY(ReplicatedUsing = OnRep_MeleeAttackCooldownReduction)
	FGameplayAttributeData MeleeAttackCooldownReduction;

	UPROPERTY(ReplicatedUsing = OnRep_RangedAttackCooldownReduction)
	FGameplayAttributeData RangedAttackCooldownReduction;

	UPROPERTY(ReplicatedUsing = OnRep_AbilityCooldownReduction)
	FGameplayAttributeData AbilityCooldownReduction;

	UPROPERTY(ReplicatedUsing = OnRep_UltimateCooldownReduction)
	FGameplayAttributeData UltimateCooldownReduction;

	UPROPERTY(ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_ReviveSpeed)
	FGameplayAttributeData ReviveSpeed;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MeleeAttackStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_RangedAttackStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AbilityStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UltimateStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MeleeAttackCooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_RangedAttackCooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AbilityCooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UltimateCooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ReviveSpeed(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
