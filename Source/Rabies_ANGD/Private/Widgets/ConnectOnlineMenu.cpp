// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ConnectOnlineMenu.h"

#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/RButton.h"

#include "Engine/World.h"

#include "Player/RMainMenuController.h"

#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Widgets/OpenLobby.h"

void UConnectOnlineMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = GetGameInstance<UEOSGameInstance>();

	ReturnBtn->RabiesButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::Return);

	CreateSessionBtn->RabiesButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::CreateSessionButtonClicked);
	FindSessionsBtn->RabiesButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::FindSessionsButtonClicked);
	SessionNameText->OnTextChanged.AddDynamic(this, &UConnectOnlineMenu::SessionNameTextChanged);
	JoinLobbyBtn->RabiesButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::JoinLobbyButtonClicked);
	JoinLobbyBtn->RabiesButton->SetIsEnabled(false);

	CreateSessionBtn->RabiesButton->SetIsEnabled(false);

	GameInst->SearchCompleted.AddUObject(this, &UConnectOnlineMenu::SessionSearchCompleted);
}

void UConnectOnlineMenu::SessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->RabiesButton->SetIsEnabled(!NewText.IsEmpty());
}

void UConnectOnlineMenu::CreateSessionButtonClicked()
{
	if (GameInst)
	{
		GameInst->CreateSession(FName{ SessionNameText->GetText().ToString() });
	}
}

void UConnectOnlineMenu::FindSessionsButtonClicked()
{
	if (GameInst)
	{
		GameInst->FindSession();
	}
}

void UConnectOnlineMenu::SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& searchResults)
{
	LobbyListScrollBox->ClearChildren();

	int index = 0;

	for (const FOnlineSessionSearchResult& searchResult : searchResults)
	{
		FString sessionName = GameInst->GetSessionName(searchResult);
		
		UOpenLobby* LobbyEntry = CreateWidget<UOpenLobby>(LobbyListScrollBox, OnlineLobbyClass);
		LobbyEntry->InitLobbyEntry(FName(sessionName), index);
		LobbyListScrollBox->AddChild(LobbyEntry);
		LobbyEntry->OnLobbyEntrySelected.AddDynamic(this, &UConnectOnlineMenu::LobbySelected);

		index++;
	}
}

void UConnectOnlineMenu::LobbySelected(int lobbyIndex)
{
	SelectedLobbyIndex = lobbyIndex;
	if (SelectedLobbyIndex != -1)
	{
		JoinLobbyBtn->RabiesButton->SetIsEnabled(true);
	}
}

void UConnectOnlineMenu::Return()
{
	ARMainMenuController* mainMenuController = Cast<ARMainMenuController>(GetWorld()->GetFirstPlayerController());

	if (mainMenuController)
	{
		mainMenuController->ChangeMainMenuState(true);
		mainMenuController->ChangeOnlineMenuState(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Return button clicked, but there is no mainmenuController sadge"));
		// Handle the case where the cast failed
	}
}

void UConnectOnlineMenu::JoinLobbyButtonClicked()
{
	if (GameInst)
	{
		GameInst->JoinLobbyBySearchIndex(SelectedLobbyIndex);
	}
}
