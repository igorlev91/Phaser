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

#include "Sound/SoundCue.h"

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

	ReturnBtn->RabiesButton->OnHovered.AddDynamic(this, &UConnectOnlineMenu::PlayHoverAudio);
	CreateSessionBtn->RabiesButton->OnHovered.AddDynamic(this, &UConnectOnlineMenu::PlayHoverAudio);
	FindSessionsBtn->RabiesButton->OnHovered.AddDynamic(this, &UConnectOnlineMenu::PlayHoverAudio);
	JoinLobbyBtn->RabiesButton->OnHovered.AddDynamic(this, &UConnectOnlineMenu::PlayHoverAudio);

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
		UGameplayStatics::PlaySound2D(this, ClickAudio);

		if (NameTable1.Num() == 0 || NameTable2.Num() == 0)
		{
			return;
		}

		// Pick random elements
		int32 Index1 = FMath::RandRange(0, NameTable1.Num() - 1);
		int32 Index2 = FMath::RandRange(0, NameTable2.Num() - 1);

		// Combine them
		FString lobbyName = NameTable1[Index1] + TEXT("_") + NameTable2[Index2];

		//GameInst->CreateSession(FName{ SessionNameText->GetText().ToString() });
		GameInst->CreateSession(FName{ lobbyName });
	}
}

void UConnectOnlineMenu::FindSessionsButtonClicked()
{
	if (GameInst)
	{
		UGameplayStatics::PlaySound2D(this, ClickAudio);

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
	UGameplayStatics::PlaySound2D(this, ClickAudio);

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
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UConnectOnlineMenu::JoinLobby, 0.2f, false);
}

void UConnectOnlineMenu::PlayHoverAudio()
{
	UGameplayStatics::PlaySound2D(this, HoverAudio);
}

void UConnectOnlineMenu::JoinLobby()
{
	if (GameInst)
	{
		GameInst->JoinLobbyBySearchIndex(SelectedLobbyIndex);
	}
}
