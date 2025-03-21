// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RRightButton.h"
#include "Player/RCharacterSelectController.h"
#include "Engine/Engine.h"

void ARRightButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RightButton clicked!"));
	}
}

void ARRightButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}
