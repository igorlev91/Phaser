// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RAbilityGenericTags.h"

FGameplayTag URAbilityGenericTags::GetGenericTargetAquiredTag()
{
	return FGameplayTag::RequestGameplayTag("targeting.aquired");
}

FGameplayTag URAbilityGenericTags::GetFullHealthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.fullHealth");
}

FGameplayTag URAbilityGenericTags::GetDeadTag()
{
	return FGameplayTag::RequestGameplayTag("stat.dead");
}

FGameplayTag URAbilityGenericTags::GetBasicAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.activate");
}

FGameplayTag URAbilityGenericTags::GetSpecialAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.special.activate");
}

FGameplayTag URAbilityGenericTags::GetUltimateAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.ultimate.activate");
}

FGameplayTag URAbilityGenericTags::GetEndScopingTag()
{
	return FGameplayTag::RequestGameplayTag("stat.endScoping");
}

FGameplayTag URAbilityGenericTags::GetScopingTag()
{
	return FGameplayTag::RequestGameplayTag("stat.scoping");
}

FGameplayTag URAbilityGenericTags::GetAttackingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.attacking");
}

FGameplayTag URAbilityGenericTags::GetEndAttackTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.endAttack");
}
