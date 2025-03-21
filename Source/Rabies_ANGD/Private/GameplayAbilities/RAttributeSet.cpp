// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAttributeSet.h"
#include "Net/UnrealNetwork.h"

void URAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		Health = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}

	if (Attribute == GetMovementSpeedAttribute())
	{
		//UE_LOG(LogTemp, Error, TEXT("Move Speed"));
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
		//UE_LOG(LogTemp, Error, TEXT("Move Speed post effect"));
	}
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

void URAttributeSet::OnRep_ReviveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URAttributeSet, ReviveSpeed, OldValue);
}

void URAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
	DOREPLIFETIME_CONDITION_NOTIFY(URAttributeSet, ReviveSpeed, COND_None, REPNOTIFY_Always);
}
