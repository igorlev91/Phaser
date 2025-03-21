// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ConnectOnlineMenu.h"
#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/Button.h"

void UConnectOnlineMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = GetGameInstance<UEOSGameInstance>();

	LoginButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::LoginButtonClicked);
	CreateSessionButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::CreateSessionButtonClicked);
	FindSessionsButton->OnClicked.AddDynamic(this, &UConnectOnlineMenu::FindSessionsButtonClicked);
	SessionNameText->OnTextChanged.AddDynamic(this, &UConnectOnlineMenu::SessionNameTextChanged);

	CreateSessionButton->SetIsEnabled(false);
}

void UConnectOnlineMenu::SessionNameTextChanged(const FText& NewText)
{
	CreateSessionButton->SetIsEnabled(!NewText.IsEmpty());
}

void UConnectOnlineMenu::LoginButtonClicked()
{
	if (GameInst)
	{
		GameInst->Login();
	}
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
