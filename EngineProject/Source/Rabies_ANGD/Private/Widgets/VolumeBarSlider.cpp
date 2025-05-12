// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VolumeBarSlider.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/RSaveGame.h"


void UVolumeBarSlider::NativeConstruct()
{
    Super::NativeConstruct();

    float VolumeValue = 1.0f; // default volume
    // Load saved volume
    USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
    if (baseSave)
    {
        URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
        if (LoadedGame)
        {
            if (isBackbground)
                VolumeValue = LoadedGame->BackgroundVolume;

            if (isSFX)
                VolumeValue = LoadedGame->SFXVolume;

            if (isVoices)
                VolumeValue = LoadedGame->VoicesVolume;

            UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
        }
    }


    volumeSlider->SetValue(VolumeValue);
    sliderProgress->SetPercent(VolumeValue);

    // Apply volume immediately
    UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixerClass, SoundClass, VolumeValue, 1.0f, 0.0f);
    UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMixerClass);

	volumeSlider->OnValueChanged.AddDynamic(this, &UVolumeBarSlider::ValueChanged);
}

void UVolumeBarSlider::ValueChanged(float newValue)
{
	sliderProgress->SetPercent(newValue);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMixerClass, SoundClass, newValue, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMixerClass);

    if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
    {
        USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
        URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
        if (LoadedGame)
        {
            if (isBackbground)
                LoadedGame->BackgroundVolume = newValue;

            if (isSFX)
                LoadedGame->SFXVolume = newValue;

            if (isVoices)
                LoadedGame->VoicesVolume = newValue;

            UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
        }
    }
    else
    {
        URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
        if (NewSave)
        {
            if (isBackbground)
                NewSave->BackgroundVolume = newValue;

            if (isSFX)
                NewSave->SFXVolume = newValue;

            if (isVoices)
                NewSave->VoicesVolume = newValue;

            UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
        }
    }
}
