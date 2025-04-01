// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/AS_BackgroundMusic.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

void AAS_BackgroundMusic::SwapPlayingMusic()
{
	if (GetAudioComponent() == nullptr)
		return;

	GetAudioComponent()->Stop();
	GetAudioComponent()->SetSound(BossAudio);
	GetAudioComponent()->Play();

}