// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RLeftButton.h"
#include "Player/RCharacterSelectController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void ARLeftButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (GEngine && CharacterSelectController)
	{
		CharacterSelectController->ConfirmCharacterChoice();
	}
}

void ARLeftButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}
