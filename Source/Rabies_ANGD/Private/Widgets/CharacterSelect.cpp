// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterSelect.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Widgets/RButton.h"

#include "Player/RMainMenuController.h"

#include "Net/UnrealNetwork.h"
#include "Framework/EOSGameState.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/RCharacterDefination.h"
#include "Player/RCharacterSelectController.h"

void UCharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCharacterSelect::SetPlayerController(ARMainMenuController* menuController)
{
	MenuController = menuController;
}
