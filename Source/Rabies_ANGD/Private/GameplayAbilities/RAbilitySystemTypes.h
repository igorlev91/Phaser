// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None                                  UMETA(DisplayName = "None"),
	Confirm                              UMETA(DisplayName = "Confirm"),
	Cancel								   UMETA(DisplayName = "Cancel"),
	BasicAttack                         UMETA(DisplayName = "Basic Attack"),
	Passive                         UMETA(DisplayName = "Passive Ability"),
	Scoping                         UMETA(DisplayName = "Scoping"),
	SpecialAttack                          UMETA(DisplayName = "Special Attack"),
	UltimateAttack						   UMETA(DisplayName = "Ultimate Attack")
};