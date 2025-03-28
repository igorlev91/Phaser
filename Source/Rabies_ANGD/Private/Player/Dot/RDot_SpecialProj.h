// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/RProjectileBase.h"
#include "RDot_SpecialProj.generated.h"

/**
 * 
 */
UCLASS()
class ARDot_SpecialProj : public ARProjectileBase
{
	GENERATED_BODY()

private:
	void HitCharacter(class ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber);
	
public:
	void Init(class UGameplayEffect effectToApply);
};
