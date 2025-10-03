// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RLoadingScreenController.h"
#include "Widgets/LoadingScreenUI.h"

ARLoadingScreenController::ARLoadingScreenController()
{
}

void ARLoadingScreenController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	PostPossessionSetup(NewPawn);
}

void ARLoadingScreenController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARLoadingScreenController::PostPossessionSetup(APawn* NewPawn)
{

}

void ARLoadingScreenController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("%s,  Got to begin play"), *GetName());
	CreateUI();
}

void ARLoadingScreenController::CreateUI()
{
	if (!LoadingScreenUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	LoadingScreenUI = CreateWidget<ULoadingScreenUI>(this, LoadingScreenUIClass);
	if (LoadingScreenUI)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  Created loading screen UI"), *GetName());
		LoadingScreenUI->AddToViewport();
	}
}