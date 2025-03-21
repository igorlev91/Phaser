// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OpenLobby.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UOpenLobby::InitLobbyEntry(const FName& LobbyName, int EntryLobbyIndex)
{
	LobbyNameText->SetText(FText::FromName(LobbyName));
	LobbyIndex = EntryLobbyIndex;
}

void UOpenLobby::LobbyButtonClicked()
{
	OnLobbyEntrySelected.Broadcast(LobbyIndex);
}

void UOpenLobby::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyButton->OnClicked.AddDynamic(this, &UOpenLobby::LobbyButtonClicked);
}
