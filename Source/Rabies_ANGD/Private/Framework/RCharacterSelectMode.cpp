// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RCharacterSelectMode.h"
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
