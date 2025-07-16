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
	ATTRIBUTE_ACCESSORS(URAttributeSet, Level)
	ATTRIBUTE_ACCESSORS(URAttributeSet, Exp)
	ATTRIBUTE_ACCESSORS(URAttributeSet, NextLevelExp)
	ATTRIBUTE_ACCESSORS(URAttributeSet, PrevLevelExp)


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
	ATTRIBUTE_ACCESSORS(URAttributeSet, Scrap)
	ATTRIBUTE_ACCESSORS(URAttributeSet, Gravity)
	ATTRIBUTE_ACCESSORS(URAttributeSet, ForwardSpeed)
	ATTRIBUTE_ACCESSORS(URAttributeSet, ReviveSpeed)
	ATTRIBUTE_ACCESSORS(URAttributeSet, BasicAttackLifesteal)
	ATTRIBUTE_ACCESSORS(URAttributeSet, DamageReduction)
	ATTRIBUTE_ACCESSORS(URAttributeSet, AbilityHealingRadius)
	ATTRIBUTE_ACCESSORS(URAttributeSet, AbilityHealingStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, DownSurvivalTime)
	ATTRIBUTE_ACCESSORS(URAttributeSet, FriendshipHealingRadius)
	ATTRIBUTE_ACCESSORS(URAttributeSet, FriendshipHealingStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, HealingRegen)
	ATTRIBUTE_ACCESSORS(URAttributeSet, TaserStunChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, NailsEffectChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, NailsEffectStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, RadioEffectChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, RadioEffectStrength)
	ATTRIBUTE_ACCESSORS(URAttributeSet, FuelCoolantEffectChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, HeaterEffectChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, UraniumEffectChance)
	ATTRIBUTE_ACCESSORS(URAttributeSet, Fire)
	ATTRIBUTE_ACCESSORS(URAttributeSet, Ice)
	ATTRIBUTE_ACCESSORS(URAttributeSet, KeyCard)
	ATTRIBUTE_ACCESSORS(URAttributeSet, TestAttribute)
	ATTRIBUTE_ACCESSORS(URAttributeSet, PassiveHealing)
	ATTRIBUTE_ACCESSORS(URAttributeSet, AirCombo)
	ATTRIBUTE_ACCESSORS(URAttributeSet, CritCombo)
	ATTRIBUTE_ACCESSORS(URAttributeSet, CritComboTimer)
	ATTRIBUTE_ACCESSORS(URAttributeSet, HealingDone)
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;

	UPROPERTY(ReplicatedUsing = OnRep_Exp)
	FGameplayAttributeData Exp;

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExp)
	FGameplayAttributeData NextLevelExp;

	UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExp)
	FGameplayAttributeData PrevLevelExp;

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

	UPROPERTY(ReplicatedUsing = OnRep_Scrap)
	FGameplayAttributeData Scrap;

	UPROPERTY(ReplicatedUsing = OnRep_Gravity)
	FGameplayAttributeData Gravity;

	UPROPERTY(ReplicatedUsing = OnRep_ForwardSpeed)
	FGameplayAttributeData ForwardSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_ReviveSpeed)
	FGameplayAttributeData ReviveSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_BasicAttackLifesteal)
	FGameplayAttributeData BasicAttackLifesteal;

	UPROPERTY(ReplicatedUsing = OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;

	UPROPERTY(ReplicatedUsing = OnRep_AbilityHealingRadius)
	FGameplayAttributeData AbilityHealingRadius;
	
	UPROPERTY(ReplicatedUsing = OnRep_AbilityHealingStrength)
	FGameplayAttributeData AbilityHealingStrength;

	UPROPERTY(ReplicatedUsing = OnRep_DownSurvivalTime)
	FGameplayAttributeData DownSurvivalTime;

	UPROPERTY(ReplicatedUsing = OnRep_FriendshipHealingRadius)
	FGameplayAttributeData FriendshipHealingRadius;

	UPROPERTY(ReplicatedUsing = OnRep_FriendshipHealingStrength)
	FGameplayAttributeData FriendshipHealingStrength;

	UPROPERTY(ReplicatedUsing = OnRep_HealingRegen)
	FGameplayAttributeData HealingRegen;

	UPROPERTY(ReplicatedUsing = OnRep_TaserStunChance)
	FGameplayAttributeData TaserStunChance;

	UPROPERTY(ReplicatedUsing = OnRep_NailsEffectChance)
	FGameplayAttributeData NailsEffectChance;

	UPROPERTY(ReplicatedUsing = OnRep_NailsEffectStrength)
	FGameplayAttributeData NailsEffectStrength;

	UPROPERTY(ReplicatedUsing = OnRep_RadioEffectChance)
	FGameplayAttributeData RadioEffectChance;

	UPROPERTY(ReplicatedUsing = OnRep_RadioEffectStrength)
	FGameplayAttributeData RadioEffectStrength;

	UPROPERTY(ReplicatedUsing = OnRep_FuelCoolantEffectChance)
	FGameplayAttributeData FuelCoolantEffectChance;

	UPROPERTY(ReplicatedUsing = OnRep_HeaterEffectChance)
	FGameplayAttributeData HeaterEffectChance;

	UPROPERTY(ReplicatedUsing = OnRep_UraniumEffectChance)
	FGameplayAttributeData UraniumEffectChance;

	UPROPERTY(ReplicatedUsing = OnRep_Fire)
	FGameplayAttributeData Fire;

	UPROPERTY(ReplicatedUsing = OnRep_Ice)
	FGameplayAttributeData Ice;

	UPROPERTY(ReplicatedUsing = OnRep_KeyCard)
	FGameplayAttributeData KeyCard;

	UPROPERTY(ReplicatedUsing = OnRep_TestAttribute)
	FGameplayAttributeData TestAttribute;

	UPROPERTY(ReplicatedUsing = OnRep_PassiveHealing)
	FGameplayAttributeData PassiveHealing;
	
	UPROPERTY(ReplicatedUsing = OnRep_AirCombo)
	FGameplayAttributeData AirCombo;

	UPROPERTY(ReplicatedUsing = OnRep_CritCombo)
	FGameplayAttributeData CritCombo;

	UPROPERTY(ReplicatedUsing = OnRep_CritComboTimer)
	FGameplayAttributeData CritComboTimer;

	UPROPERTY(ReplicatedUsing = OnRep_HealingDone)
	FGameplayAttributeData HealingDone;

	UFUNCTION()
	void OnRep_PassiveHealing(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_TestAttribute(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Exp(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NextLevelExp(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PrevLevelExp(const FGameplayAttributeData& OldValue);

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
	void OnRep_Scrap(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Gravity(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ForwardSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ReviveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BasicAttackLifesteal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DamageReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AbilityHealingRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AbilityHealingStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DownSurvivalTime(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FriendshipHealingRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FriendshipHealingStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealingRegen(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_TaserStunChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NailsEffectChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NailsEffectStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_RadioEffectChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_RadioEffectStrength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FuelCoolantEffectChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HeaterEffectChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UraniumEffectChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Fire(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Ice(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_KeyCard(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AirCombo(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CritCombo(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CritComboTimer(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealingDone(const FGameplayAttributeData& OldValue);

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
