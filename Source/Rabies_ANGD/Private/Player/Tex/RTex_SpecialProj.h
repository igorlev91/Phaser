// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/RProjectileBase.h"
#include "RTex_SpecialProj.generated.h"

/**
 * 
 */
UCLASS()
class ARTex_SpecialProj : public ARProjectileBase
{
	GENERATED_BODY()
	
private:
	void HitCharacter(class ARCharacterBase* usingCharacter, class ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber);

	TArray<TSubclassOf<class UGameplayEffect>> EffectsToApply;

	TArray<class ARCharacterBase*> AlreadyHitCharacters;

public:
	void Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply);
};
