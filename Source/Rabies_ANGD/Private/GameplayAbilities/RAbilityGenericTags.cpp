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

FGameplayTag URAbilityGenericTags::GetUnActionableTag()
{
	return FGameplayTag::RequestGameplayTag("stat.unActionable");
}

FGameplayTag URAbilityGenericTags::GetFlyingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.flying");
}

FGameplayTag URAbilityGenericTags::GetEndFlyingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.endFlying");
}

FGameplayTag URAbilityGenericTags::GetEndTakeOffChargeTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.endTakeOffCharge");
}

FGameplayTag URAbilityGenericTags::GetTakeOffDelayTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.TakeOffDelay");
}

FGameplayTag URAbilityGenericTags::GetApplyGravityJump()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.applyGravityJump");
}

FGameplayTag URAbilityGenericTags::GetRemoveGravityJump()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.removeGravityJump");
}
