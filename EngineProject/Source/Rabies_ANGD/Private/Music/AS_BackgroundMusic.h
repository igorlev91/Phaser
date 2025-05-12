// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/AmbientSound.h"
#include "AS_BackgroundMusic.generated.h"

/**
 * 
 */
UCLASS()
class AAS_BackgroundMusic : public AAmbientSound
{
	GENERATED_BODY()
public:
	void SwapPlayingMusic();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* BossAudio;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

};
