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
	static FGameplayTag GetSpecialAttackActivationTag();
	static FGameplayTag GetUltimateAttackActivationTag();
	static FGameplayTag GetScopingTag();

};
