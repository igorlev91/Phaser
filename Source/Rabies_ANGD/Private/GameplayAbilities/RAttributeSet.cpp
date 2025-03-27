// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAttributeSet.h"
#include "Net/UnrealNetwork.h"

void URAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		Health = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}

	if (Attribute == GetAbilityCooldownReductionAttribute())
	{
		AbilityCooldownReduction = FMath::Clamp(NewValue, 0.1f, 1.0f);
	}

	if (Attribute == GetMeleeAttackCooldownReductionAttribute())
	{
		MeleeAttackCooldownReduction = FMath::Clamp(NewValue, 0.1f, 1.0f);
	}

	if (Attribute == GetRangedAttackCooldownReductionAttribute())
	{
		RangedAttackCooldownReduction = FMath::Clamp(NewValue, 0.1f, 1.0f);
	}

	if (Attribute == GetUltimateCooldownReductionAttribute())
	{
		UltimateCooldownReduction = FMath::Clamp(NewValue, 0.1f, 1.0f);
	}
}

void URAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{

	}
}

void URAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, Level, OldValue);
}

void URAttributeSet::OnRep_Exp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, Exp, OldValue);
}

void URAttributeSet::OnRep_NextLevelExp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, NextLevelExp, OldValue);
}

void URAttributeSet::OnRep_PrevLevelExp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, PrevLevelExp, OldValue);
}

void URAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, Health, OldValue);
}

void URAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, MaxHealth, OldValue);
}

void URAttributeSet::OnRep_MeleeAttackStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, MeleeAttackStrength, OldValue);
}

void URAttributeSet::OnRep_RangedAttackStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, RangedAttackStrength, OldValue);
}

void URAttributeSet::OnRep_AbilityStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, AbilityStrength, OldValue);
}

void URAttributeSet::OnRep_UltimateStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, UltimateStrength, OldValue);
}

void URAttributeSet::OnRep_MeleeAttackCooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, MeleeAttackCooldownReduction, OldValue);
}

void URAttributeSet::OnRep_RangedAttackCooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, RangedAttackCooldownReduction, OldValue);
}

void URAttributeSet::OnRep_AbilityCooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, AbilityCooldownReduction, OldValue);
}

void URAttributeSet::OnRep_UltimateCooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, UltimateCooldownReduction, OldValue);
}

void URAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, MovementSpeed, OldValue);
}

void URAttributeSet::OnRep_Scrap(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, Scrap, OldValue);
}

void URAttributeSet::OnRep_Gravity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, Gravity, OldValue);
}

void URAttributeSet::OnRep_ReviveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, ReviveSpeed, OldValue);
}

void URAttributeSet::OnRep_BasicAttackLifesteal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, BasicAttackLifesteal, OldValue);
}

void URAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, DamageReduction, OldValue);
}

void URAttributeSet::OnRep_AbilityHealingRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, AbilityHealingRadius, OldValue);
}

void URAttributeSet::OnRep_AbilityHealingStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, AbilityHealingStrength, OldValue);
}

void URAttributeSet::OnRep_DownSurvivalTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, DownSurvivalTime, OldValue);
}

void URAttributeSet::OnRep_FriendshipHealingRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, FriendshipHealingRadius, OldValue);
}

void URAttributeSet::OnRep_FriendshipHealingStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, FriendshipHealingStrength, OldValue);
}

void URAttributeSet::OnRep_HealingRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, HealingRegen, OldValue);
}

void URAttributeSet::OnRep_TaserStunChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, TaserStunChance, OldValue);
}

void URAttributeSet::OnRep_NailsEffectChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, NailsEffectChance, OldValue);
}

void URAttributeSet::OnRep_NailsEffectStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, NailsEffectStrength, OldValue);
}

void URAttributeSet::OnRep_RadioEffectChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, RadioEffectChance, OldValue);
}

void URAttributeSet::OnRep_RadioEffectStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, RadioEffectStrength, OldValue);
}

void URAttributeSet::OnRep_FuelCoolantEffectChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, FuelCoolantEffectChance, OldValue);
}

void URAttributeSet::OnRep_HeaterEffectChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, HeaterEffectChance, OldValue);
}

void URAttributeSet::OnRep_UraniumEffectChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, UraniumEffectChance, OldValue);
}

void URAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, NextLevelExp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, PrevLevelExp, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, MeleeAttackStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, RangedAttackStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, AbilityStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, UltimateStrength, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, MeleeAttackCooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, RangedAttackCooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, AbilityCooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, UltimateCooldownReduction, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, Scrap, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, Gravity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, ReviveSpeed, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, BasicAttackLifesteal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, AbilityHealingRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, AbilityHealingStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, DownSurvivalTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, FriendshipHealingRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, FriendshipHealingStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, HealingRegen, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, TaserStunChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, NailsEffectChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, NailsEffectStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, RadioEffectChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, RadioEffectStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, FuelCoolantEffectChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, HeaterEffectChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, UraniumEffectChance, COND_None, REPNOTIFY_Always);
}
