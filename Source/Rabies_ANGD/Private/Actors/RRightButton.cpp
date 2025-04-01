// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RRightButton.h"
#include "Actors/Clipboard.h"
#include "Player/RCharacterSelectController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Framework/EOSGameState.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

void ARRightButton::EnableClick()
{
	SetLight(true);
	bPressDelay = false;

}

ARRightButton::ARRightButton()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
}

void ARRightButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (AudioComp && SelectAudio && bPlaySound)
	{
		AudioComp->SetSound(SelectAudio);
		AudioComp->Play();

		bPlaySound = false;
	}

	if (GEngine && bPressDelay == false)
	{
		GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
		if (GameState)
		{
			// Successfully got the game mode
			SetLight(false);
			bPressDelay = true;

			CharacterSelectController = Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController());
			if (CharacterSelectController != nullptr)
			{
				CharacterSelectController->NextCharacter(this);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RightButton clicked!"));
		}

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARRightButton::SetPlaySoundToTrue, 1.0f, false);
	}
}

void ARRightButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}

void ARRightButton::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClipboard::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		Clipboard = Cast<AClipboard>(FoundActors[0]);
	}
}

void ARRightButton::SetPlaySoundToTrue()
{
	bPlaySound = true;
}
