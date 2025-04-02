// Fill out your copyright notice in the Description page of Project Settings.


#include "CandyMainMenu.h"
#include "../../CandyChaosLobbyGameInstance.h"
#include "../../Common/Library/CandyChaosLoobyBlueprintLibrary.h"
#include <Kismet/GameplayStatics.h>

bool UCandyMainMenu::Initialize()
{
    if (!Super::Initialize())
        return false;

    if (HostButton && FindGamesButton && OptionsButton && ExitButton) {
        HostButton->OnClicked().AddUObject(this, &ThisClass::OnHostButtonClicked);
        FindGamesButton->OnClicked().AddUObject(this, &ThisClass::OnFindGamesButtonClicked);
        OptionsButton->OnClicked().AddUObject(this, &ThisClass::OnOptionsButtonButtonClicked);
        ExitButton->OnClicked().AddUObject(this, &ThisClass::OnExitButtonClicked);
    }

    this->CandyChaosLobbyGameInstance = UCandyChaosLoobyBlueprintLibrary::GetCandyChaosLobbyGameInstance(this);

    return true;
}

void UCandyMainMenu::NativeDestruct()
{
    TearDown();

    Super::NativeDestruct();
}

void UCandyMainMenu::OnHostButtonClicked()
{
    this->CandyChaosLobbyGameInstance->SetHostGame(true);
    this->CandyChaosLobbyGameInstance->SetFindGames(false);
    this->CandyChaosLobbyGameInstance->SetOptionsMenu(false);

    UGameplayStatics::OpenLevel(GWorld, "MainMenuWidgets", true);
}

void UCandyMainMenu::OnFindGamesButtonClicked()
{
    this->CandyChaosLobbyGameInstance->SetFindGames(true);
    this->CandyChaosLobbyGameInstance->SetHostGame(false);
    this->CandyChaosLobbyGameInstance->SetOptionsMenu(false);

    UGameplayStatics::OpenLevel(GWorld, "MainMenuWidgets", true);
}

void UCandyMainMenu::OnOptionsButtonButtonClicked()
{
   this->CandyChaosLobbyGameInstance->SetOptionsMenu(true);
    this->CandyChaosLobbyGameInstance->SetHostGame(false);
    this->CandyChaosLobbyGameInstance->SetFindGames(false);

    UGameplayStatics::OpenLevel(GWorld, "MainMenuWidgets", true);
}

void UCandyMainMenu::OnExitButtonClicked() {

    UWorld* World = GetWorld();

    if (World == nullptr) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();

    if (PlayerController == nullptr) return;

    PlayerController->ConsoleCommand("quit");
}