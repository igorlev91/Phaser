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
#include "Widgets/SelectAbilityIcon.h"
#include "Widgets/ClipboardInfo.h"
#include "Actors/Clipboard.h"

void ARCharacterSelectMode::InitGameState()
{
	Super::InitGameState();

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ARCharacterSelectMode::HandlePostMapLoad);
}

void ARCharacterSelectMode::HandlePostMapLoad(UWorld* LoadedWorld)
{
	if (LoadedWorld)
	{
		AEOSGameState* EOSGameState = Cast<AEOSGameState>(LoadedWorld->GetGameState());
		UEOSGameInstance* GameInstance = Cast<UEOSGameInstance>(LoadedWorld->GetGameInstance());

		if (EOSGameState && GameInstance)
		{
			EOSGameState->SetSessionName(GameInstance->GetCurrentSessionName());
		}
	}
}