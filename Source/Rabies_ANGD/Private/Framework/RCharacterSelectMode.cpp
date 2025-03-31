// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RCharacterSelectMode.h"
#include "Framework/RCharacterDefination.h"
#include "Actors/CagedCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Player/RCharacterSelectController.h"
#include "Actors/RRightButton.h"
#include "Algo/Sort.h"
#include "EOSGameState.h"
#include "EOSGameInstance.h"

void ARCharacterSelectMode::InitGameState()
{
	Super::InitGameState();

	AEOSGameState* gameState = Cast<AEOSGameState>(GameState);
	UEOSGameInstance* gameInstance = GetGameInstance<UEOSGameInstance>();

	if (gameState && gameInstance)
	{
		gameState->SetSessionName(gameInstance->GetCurrentSessionName());
	}
}
