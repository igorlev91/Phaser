// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/RProjectileBase.h"
#include "Sound/SoundAttenuation.h"
#include "RChester_UltimateProj.generated.h"

/**
 * 
 */
UCLASS()
class ARChester_UltimateProj : public ARProjectileBase
{
	GENERATED_BODY()
	
public:
	ARChester_UltimateProj();

private:
	void HitCharacter(class ARCharacterBase* usingCharacter, class ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber);

	TArray<TSubclassOf<class UGameplayEffect>> EffectsToApply;

	TArray<class ARCharacterBase*> AlreadyHitCharacters;

	void SelfDestroyExplosion();

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HitExplosion;

	void Explosion(ARCharacterBase* usingCharacter);

	class ARCharacterBase* UsingCharacter;

	bool bNoMoreExplosion;

	FTimerHandle NewDestroyHandle;
	FTimerHandle EnableTimerHandle;

	void EnableHitDetection();

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

public:
	void Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
