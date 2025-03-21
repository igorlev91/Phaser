// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RCharacterSelectController.h"
#include "Framework/EOSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CharacterSelect.h"

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

	CreateCharacterSelectUI();
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
