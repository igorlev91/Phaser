// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerController.h"
#include "Player/RPlayerBase.h"
#include "Widgets/GameplayUI.h"

void ARPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
	if (PlayerBase)
	{
		//PlayerCharacter->InitAbilities();
	}
}

void ARPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARPlayerController::PostPossessionSetup(APawn* NewPawn)
{
	PlayerBase = Cast<ARPlayerBase>(NewPawn);
	if (!PlayerBase)
	{
		UE_LOG(LogTemp, Error, TEXT("Needs CPlayerCharacter as Pawn"));
		return;
	}

	// Set up the players ability system

	CreateGameplayUI();
	if (IsLocalPlayerController())
	{
		// Set up the players initial attributes
	}
}

void ARPlayerController::CreateGameplayUI()
{
	if (!PlayerBase)
	{
		return;
	}

	if (!GameplayUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}
	//return true, if this player controller represent a human player sitting infront of this manchine.
	if (!IsLocalPlayerController())
	{
		return;
	}

	GameplayUI = CreateWidget<UGameplayUI>(this, GameplayUIClass);
	if (GameplayUI)
	{
		GameplayUI->AddToViewport();
	}
}
