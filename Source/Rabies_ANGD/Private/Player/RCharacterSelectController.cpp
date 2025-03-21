// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RCharacterSelectController.h"
#include "Framework/EOSGameInstance.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/EOSGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Widgets/CharacterSelect.h"
#include "Framework/RCharacterDefination.h"

void ARCharacterSelectController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BP_OnRep_PlayerState();
	
	if (IsLocalPlayerController()) //maybe also check if they have authority?
	{
		myPlayerID = GetPlayerID();
		CreateCharacterSelectUI();
	}
}

void ARCharacterSelectController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
}

void ARCharacterSelectController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	CurrentlyHoveredCharacter = GameState->GetDefinationFromIndex(0);
}

void ARCharacterSelectController::ConfirmCharacterChoice()
{
	if (!IsLocalPlayerController())
		return;

	AEOSPlayerState* playerState = Cast<AEOSPlayerState>(PlayerState);
	if (playerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}
	playerState->Server_CharacterSelected(CurrentlyHoveredCharacter);
}

int ARCharacterSelectController::GetPlayerID()
{
	for (int i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		//AEOSPlayerState* currentPlayer = Cast<AEOSPlayerState>(GameState->PlayerArray[i]);
		//if (myPlayerID == GameState->PlayerArray[i].)
		//{
		//	return i;
		//}
	}

	return 1;
}

void ARCharacterSelectController::PostPossessionSetup(APawn* NewPawn)
{
}

void ARCharacterSelectController::CreateCharacterSelectUI()
{
	if (!CharacterSelectUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	CharacterSelectUI = CreateWidget<UCharacterSelect>(this, CharacterSelectUIClass);
	if (CharacterSelectUI)
	{
		CharacterSelectUI->AddToViewport();
	}
}
