// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterSelect.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Widgets/RButton.h"

#include "Net/UnrealNetwork.h"
#include "Framework/EOSGameState.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/RCharacterDefination.h"
#include "Player/RCharacterSelectController.h"

void UCharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	ReadyUpButton->RabiesButton->OnClicked.AddDynamic(this, &UCharacterSelect::ReadyUp);
}

void UCharacterSelect::ReadyUp()
{
	// do your checks here
	UE_LOG(LogTemp, Warning, TEXT("Ready up"));
	LoadMapAndListen(GameLevel);
}

void UCharacterSelect::LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad)
{
	if (!levelToLoad.IsValid())
	{
		levelToLoad.LoadSynchronous();
	}

	if (levelToLoad.IsValid())
	{
		const FName levelName = FName(*FPackageName::ObjectPathToPackageName(levelToLoad.ToString()));
		GetWorld()->ServerTravel(levelName.ToString() + "?listen");
	}
}
