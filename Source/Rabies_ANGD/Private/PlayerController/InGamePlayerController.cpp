// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerController.h"


void AInGamePlayerController::SetCurrentCharacter(ARCharacterBase* InCurrentCharacter)
{
    this->CurrentCharacter = InCurrentCharacter;
}

ARCharacterBase* AInGamePlayerController::GetCurrentCharacter()
{
    return this->CurrentCharacter;
}
