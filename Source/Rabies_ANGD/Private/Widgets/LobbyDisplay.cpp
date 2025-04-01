// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyDisplay.h"
#include "Components/TextBlock.h"
#include "Framework/EOSGameState.h"
#include "Engine/World.h"
#include "Components/ListView.h"
#include "Framework/RCharacterDefination.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TileView.h"
#include "Widgets/PlayerEntry.h"
#include "Engine/Engine.h"
#include "Player/RCharacterSelectController.h"
#include "Framework/EOSPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Widgets/CharacterEntry.h"

void ULobbyDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));


	if (GameState)
	{
		FName lobbyName = GameState->GetSessionName();
		LobbyName->SetText(FText::FromName(lobbyName));
		GameState->OnSessionNameReplicated.AddDynamic(this, &ULobbyDisplay::SessionNameReplicated);
	
		LobbyPlayerList->SetListItems(GameState->PlayerArray);
		CharacterListIcon->SetListItems(GameState->PlayerArray);

		FString displayNumText = FString(LobbyPlayerList->GetNumItems() + "/4");
		LobbyPlayerNumber->SetText(FText::FromString(displayNumText));

		GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateHandle, this, &ULobbyDisplay::RefreshPlayerList, 1, true);


		GameState->OnCharacterSelectionReplicated.AddDynamic(this, &ULobbyDisplay::CharacterSelectionReplicated);
	}
}

void ULobbyDisplay::SessionNameReplicated(const FName& newSessionName)
{
	LobbyName->SetText(FText::FromName(newSessionName));
}

void ULobbyDisplay::RefreshPlayerList()
{
	if (GameState)
	{
		LobbyPlayerList->SetListItems(GameState->PlayerArray);
		CharacterListIcon->SetListItems(GameState->PlayerArray);

		FString displayNumText = FString(LobbyPlayerList->GetNumItems() + "/4");
		LobbyPlayerNumber->SetText(FText::FromString(displayNumText));
	}
}

void ULobbyDisplay::CharacterSelectionReplicated(const URCharacterDefination* selected, const URCharacterDefination* deselcted, const FString& playerNameCheck)
{
	ARCharacterSelectController* playerSelectControler = Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController());

	if (selected != nullptr && playerSelectControler != nullptr && LobbyPlayerList != nullptr)
	{
		for (int i = 0; i < LobbyPlayerList->GetDisplayedEntryWidgets().Num(); i++)
		{
			UPlayerEntry* playerEntry = Cast<UPlayerEntry>(LobbyPlayerList->GetDisplayedEntryWidgets()[i]);
			if (playerEntry == nullptr)
				continue;

			if (playerEntry->GetPlayerName() == playerNameCheck)
			{
				UCharacterEntry* characterEntry = Cast<UCharacterEntry>(CharacterListIcon->GetDisplayedEntryWidgets()[i]);
				if (characterEntry)
				{
					characterEntry->SetCharacterIcon(selected);
				}
			}
		}
	}
	
	if (deselcted != nullptr)
	{
		// keeping this in here for when we need to repicate the cages and look of the game
	}
}
