
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "RAbilityGenericTags.generated.h"

/**
 * 
 */
UCLASS()
class URAbilityGenericTags : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FGameplayTag GetGenericTargetAquiredTag();
	static FGameplayTag GetFullHealthTag();
	static FGameplayTag GetDeadTag();
	static FGameplayTag GetBasicAttackActivationTag();
	static FGameplayTag GetMeleeAttackCooldown();
	static FGameplayTag GetRangedAttackCooldown();
	static FGameplayTag GetSpecialAttackActivationTag();
	static FGameplayTag GetSpecialAttackAimingTag();
	static FGameplayTag GetSpecialAttackCooldown();
	static FGameplayTag GetUltimateAttackActivationTag();
	static FGameplayTag GetUltimateAttackAimingTag();
	static FGameplayTag GetUltimateAttackCooldown();
	static FGameplayTag GetScopingTag();
	static FGameplayTag GetEndScopingTag();
	static FGameplayTag GetAttackingTag();
	static FGameplayTag GetStartAimingTag();
	static FGameplayTag GetEndAimingTag();
	static FGameplayTag GetEndAttackTag();
	static FGameplayTag GetUnActionableTag();
	static FGameplayTag GetFlyingTag();
	static FGameplayTag GetTiredFlyingTag();
	static FGameplayTag GetTakeOffDelayTag();
	static FGameplayTag GetEndTakeOffChargeTag();
	static FGameplayTag GetApplyGravityJump();
	static FGameplayTag GetHoldingJump();

	static FGameplayTag GetScrapTag();
	static FGameplayTag GetRevivingTag();
	static FGameplayTag GetReviveTag();
	static FGameplayTag GetEndRevivingTag();

	static FGameplayTag GetLevelTag();
	static FGameplayTag GetExpTag();
	static FGameplayTag GetNextLevelTag();
	static FGameplayTag GetPrevLevelTag();
	static FGameplayTag GetMaxHealthTag();
	static FGameplayTag GetHealthTag();
	static FGameplayTag GetMeleeAttackStrengthTag();
	static FGameplayTag GetRangedAttackStrengthTag();

};
