// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VolumeBarSlider.generated.h"

/**
 * 
 */
UCLASS()
class UVolumeBarSlider : public UUserWidget
{
	GENERATED_BODY()
	

public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ValueChanged(float newValue);

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool isBackbground;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool isSFX;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool isVoices;

	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundMix* SoundMixerClass;

	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundClass* SoundClass;

	UPROPERTY(meta = (BindWidget))
	class USlider* volumeSlider;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* sliderProgress;
};
