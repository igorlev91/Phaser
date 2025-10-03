// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/RProjectileBase.h"
#include "RBoltHead_RangedProj.generated.h"

/**
 * 
 */
UCLASS()
class ARBoltHead_RangedProj : public ARProjectileBase
{
	GENERATED_BODY()
	
private:
	void HitCharacter(class ARCharacterBase* usingCharacter, class ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber);

	TArray<TSubclassOf<class UGameplayEffect>> EffectsToApply;

	TArray<class ARCharacterBase*> AlreadyHitCharacters;

	UFUNCTION()
	void Explosion();

	UFUNCTION()
	void BounceCharacter(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HitExplosion;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

	class ARCharacterBase* UsingCharacter;

	bool bNoMoreExplosion;

	FTimerHandle EnableTimerHandle;

	void EnableHitDetection();

public:
	void Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
