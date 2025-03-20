// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilities/RGameplayCueInterface.h"

#include "GenericTeamAgentInterface.h"

#include "RCharacterBase.generated.h"

class URAbilitySystemComponent;
class URAttributeSet;
class UGameplayEffect;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeadStatusChanged, bool /*bIsDead*/);

UCLASS()
class ARCharacterBase : public ACharacter//, public IAbilitySystemInterface, public IRGameplayCueInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FOnDeadStatusChanged OnDeadStatusChanged;

	// Sets default values for this character's properties
	ARCharacterBase();

};