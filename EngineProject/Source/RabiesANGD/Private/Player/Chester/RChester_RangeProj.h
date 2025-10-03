// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/RProjectileBase.h"
#include "RChester_RangeProj.generated.h"

/**
 * 
 */
UCLASS()
class ARChester_RangeProj : public ARProjectileBase
{
	GENERATED_BODY()

private:
	void HitCharacter(class ARCharacterBase* usingCharacter, class ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber);

	TArray<TSubclassOf<class UGameplayEffect>> EffectsToApply;

	TArray<class ARCharacterBase*> AlreadyHitCharacters;

	UFUNCTION()
	void BounceCharacter(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

public:
	void Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter);
};
