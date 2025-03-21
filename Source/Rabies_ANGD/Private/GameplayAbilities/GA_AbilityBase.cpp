// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UGA_AbilityBase::UGA_AbilityBase()
{
	ActivationBlockedTags.AddTag(URAbilityGenericTags::GetDeadTag());
}