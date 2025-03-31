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

void ARRightButton::EnableClick()
{
	SetLight(true);
	bPressDelay = false;

}

void ARRightButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (GEngine && bPressDelay == false)
	{
		GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
		if (GameState)
		{
			// Successfully got the game mode
			SetLight(false);
			bPressDelay = true;

			CharacterSelectController->NextCharacter(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RightButton clicked!"));
		}
	}
}

void ARRightButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}

void ARRightButton::BeginPlay()
{
	Super::BeginPlay();

	CharacterSelectController = Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController());

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClipboard::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		Clipboard = Cast<AClipboard>(FoundActors[0]);
	}
}
