// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyDisplay.h"
#include "Components/TextBlock.h"
#include "Framework/EOSGameState.h"
#include "Engine/World.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

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

		GetWorld()->GetTimerManager().SetTimer(PlayerListUpdateHandle, this, &ULobbyDisplay::RefreshPlayerList, 1, true);
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

		FString displayNumText = FString(LobbyPlayerList->GetNumItems() + "/4");
		LobbyPlayerNumber->SetText(FText::FromString(displayNumText));
	}
}
