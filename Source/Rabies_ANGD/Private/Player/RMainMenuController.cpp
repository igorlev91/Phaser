// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RMainMenuController.h"
#include "Framework/EOSGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/MainMenu.h"

void ARMainMenuController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
}

void ARMainMenuController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

	GetGameInstance<UEOSGameInstance>()->SetMenuController(this);
	CreateMenuUI();
}

void ARMainMenuController::ChangeOnlineMenuState(bool state)
{
	if (MenuUI)
	{
		MenuUI->ChangeConnectMenuState(state);
	}
}

void ARMainMenuController::ChangeMainMenuState(bool state)
{
	if (MenuUI)
	{
		MenuUI->ChangeMainMenuState(state);
	}
}

void ARMainMenuController::PostPossessionSetup(APawn* NewPawn)
{

}

void ARMainMenuController::CreateMenuUI()
{
	if (!MenuUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	MenuUI = CreateWidget<UMainMenu>(this, MenuUIClass);
	if (MenuUI)
	{
		MenuUI->AddToViewport();
	}
}
