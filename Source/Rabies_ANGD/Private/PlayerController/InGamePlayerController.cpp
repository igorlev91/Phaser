// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerController.h"


void AInGamePlayerController::SetCurrentCharacter(ARLobbyCharacter* InCurrentCharacter)
{
    this->CurrentCharacter = InCurrentCharacter;
}

ARLobbyCharacter* AInGamePlayerController::GetCurrentCharacter()
{
    return this->CurrentCharacter;
}
