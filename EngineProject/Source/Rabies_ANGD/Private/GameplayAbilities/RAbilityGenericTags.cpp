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

FGameplayTag URAbilityGenericTags::GetFullyDeadTag()
{
	return FGameplayTag::RequestGameplayTag("stat.fullyDead");
}

FGameplayTag URAbilityGenericTags::GetBasicAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.activate");
}

FGameplayTag URAbilityGenericTags::GetRangedAttackLeftActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.rangedLeft");
}

FGameplayTag URAbilityGenericTags::GetRangedAttackRightActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.rangedRight");
}

FGameplayTag URAbilityGenericTags::GetMeleeAttackCooldown()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.meleeAtkCooldown");
}

FGameplayTag URAbilityGenericTags::GetRangedAttackCooldown()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.rangedAtkCooldown");
}

FGameplayTag URAbilityGenericTags::GetSpecialAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.special.activate");
}

FGameplayTag URAbilityGenericTags::GetSpecialAttackAimingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.special.aiming");
}

FGameplayTag URAbilityGenericTags::GetSpecialAttackCooldown()
{
	return FGameplayTag::RequestGameplayTag("ability.special.cooldown");
}

FGameplayTag URAbilityGenericTags::GetUltimateAttackActivationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.ultimate.activate");
}

FGameplayTag URAbilityGenericTags::GetUltimateAttackAimingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.ultimate.aiming");
}

FGameplayTag URAbilityGenericTags::GetUltimateAttackCooldown()
{
	return FGameplayTag::RequestGameplayTag("ability.ultimate.cooldown");
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

FGameplayTag URAbilityGenericTags::GetMeleeAttackingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.meleeAttacking");
}

FGameplayTag URAbilityGenericTags::GetStartAimingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.startAiming");
}

FGameplayTag URAbilityGenericTags::GetEndAimingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.endAiming");
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

FGameplayTag URAbilityGenericTags::GetInvisTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.invis");
}

FGameplayTag URAbilityGenericTags::GetTiredFlyingTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.tiredFlying");
}

FGameplayTag URAbilityGenericTags::GetTakeOffDelayTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.TakeOffDelay");
}

FGameplayTag URAbilityGenericTags::GetEndTakeOffChargeTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.endTakeOffDelay");;
}

FGameplayTag URAbilityGenericTags::GetApplyGravityJump()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.applyGravityJump");
}

FGameplayTag URAbilityGenericTags::GetHoldingJump()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.holdingJump");
}

FGameplayTag URAbilityGenericTags::GetScrapTag()
{
	return FGameplayTag::RequestGameplayTag("stat.scrap");
}

FGameplayTag URAbilityGenericTags::GetTaserTag()
{
	return FGameplayTag::RequestGameplayTag("stat.taser");
}

FGameplayTag URAbilityGenericTags::GetRadiationTag()
{
	return FGameplayTag::RequestGameplayTag("stat.radiation");
}

FGameplayTag URAbilityGenericTags::GetFireTag()
{
	return FGameplayTag::RequestGameplayTag("stat.fire");
}

FGameplayTag URAbilityGenericTags::GetIceTag()
{
	return FGameplayTag::RequestGameplayTag("stat.ice");
}

FGameplayTag URAbilityGenericTags::GetRevivingTag()
{
	return FGameplayTag::RequestGameplayTag("stat.reviving");
}

FGameplayTag URAbilityGenericTags::GetReviveTag()
{
	return FGameplayTag::RequestGameplayTag("stat.revive");
}

FGameplayTag URAbilityGenericTags::GetEndRevivingTag()
{
	return FGameplayTag::RequestGameplayTag("stat.endReviving");
}

FGameplayTag URAbilityGenericTags::GetLevelTag()
{
	return FGameplayTag::RequestGameplayTag("stat.level");
}

FGameplayTag URAbilityGenericTags::GetExpTag()
{
	return FGameplayTag::RequestGameplayTag("stat.exp");
}

FGameplayTag URAbilityGenericTags::GetNextLevelTag()
{
	return FGameplayTag::RequestGameplayTag("stat.nextLevelExp");
}

FGameplayTag URAbilityGenericTags::GetPrevLevelTag()
{
	return FGameplayTag::RequestGameplayTag("stat.prevLevelExp");
}

FGameplayTag URAbilityGenericTags::GetMaxHealthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.maxHealth");
}

FGameplayTag URAbilityGenericTags::GetHealthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.health");
}

FGameplayTag URAbilityGenericTags::GetMeleeAttackStrengthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.meleeAttackStrength");
}

FGameplayTag URAbilityGenericTags::GetRangedAttackStrengthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.rangedAttackStrength");
}

FGameplayTag URAbilityGenericTags::GetSpecialStrengthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.specialStrength");
}

FGameplayTag URAbilityGenericTags::GetUltimateStrengthTag()
{
	return FGameplayTag::RequestGameplayTag("stat.ultimateStrength");
}

FGameplayTag URAbilityGenericTags::GetCooldownRefreshTag()
{
	return FGameplayTag::RequestGameplayTag("ability.widget.refreshCooldown");
}

FGameplayTag URAbilityGenericTags::GetShowRollerTag()
{
	return FGameplayTag::RequestGameplayTag("stat.showRoller");
}

FGameplayTag URAbilityGenericTags::GetHideRollerTag()
{
	return FGameplayTag::RequestGameplayTag("stat.hideRoller");
}

FGameplayTag URAbilityGenericTags::GetRollUpTag()
{
	return FGameplayTag::RequestGameplayTag("stat.rollUp");
}

FGameplayTag URAbilityGenericTags::GetUnrollTag()
{
	return FGameplayTag::RequestGameplayTag("stat.unroll");
}

FGameplayTag URAbilityGenericTags::GetImmuneTag()
{
	return FGameplayTag::RequestGameplayTag("stat.immune");
}

FGameplayTag URAbilityGenericTags::GetDefeatedEnemyTag()
{
	return FGameplayTag::RequestGameplayTag("ability.defeatedEnemy");
}

FGameplayTag URAbilityGenericTags::GetUltimateHold()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.HoldUltimate");
}

FGameplayTag URAbilityGenericTags::GetUltimateRelease()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.ReleaseUltimate");
}

FGameplayTag URAbilityGenericTags::GetComboChange()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.combo");
}

FGameplayTag URAbilityGenericTags::GetComboEnd()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.comboEnd");
}

FGameplayTag URAbilityGenericTags::GetComboActivate()
{
	return FGameplayTag::RequestGameplayTag("ability.attack.comboActivate");
}
