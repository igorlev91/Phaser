// Fill out your copyright notice in the Description page of Project Settings.


#include "CandyChaosLobbyGameInstance.h"
#include "TimerManager.h"
#include "Runtime/UMG/Public/UMG.h"
//#include "CandyChaosLobbyLoadingScreen.h"
#include "Net/UnrealNetwork.h"
#include <Kismet/GameplayStatics.h>

UCandyChaosLobbyGameInstance::UCandyChaosLobbyGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

UCandyMainMenu* UCandyChaosLobbyGameInstance::LoadMenu()
{
    if (!ensure(MenuClass != nullptr)) return nullptr;
    this->Menu = CreateWidget<UCandyMainMenu>(this, this->MenuClass);

    if (!ensure(Menu != nullptr)) return nullptr;

    this->Menu->Setup();

    return this->Menu;
}

void UCandyChaosLobbyGameInstance::SetHostGame(bool InbIsHostGameMenu)
{
    this->bIsHostGameMenu = InbIsHostGameMenu;
}

void UCandyChaosLobbyGameInstance::SetFindGames(bool InbIsFindGamesMenu)
{
    this->bIsFindGamesMenu = InbIsFindGamesMenu;
}

void UCandyChaosLobbyGameInstance::SetOptionsMenu(bool InbIsOptionsMenu)
{
    this->bIsOptionsMenu = InbIsOptionsMenu;
}

void UCandyChaosLobbyGameInstance::SetBackToMainMenu(bool InbIsBackToMainMenu)
{
    this->bIsBackToMainMenu = InbIsBackToMainMenu;
}

bool UCandyChaosLobbyGameInstance::GetHostGame()
{
    return this->bIsHostGameMenu;
}

bool UCandyChaosLobbyGameInstance::GetFindGames()
{
    return this->bIsFindGamesMenu;
}

bool UCandyChaosLobbyGameInstance::GetOptionsMenu()
{
    return this->bIsOptionsMenu;
}

bool UCandyChaosLobbyGameInstance::GetBackToMainMenu()
{
    return this->bIsBackToMainMenu;
}

void UCandyChaosLobbyGameInstance::SetHostSettings(int32 InNumberOfPlayers, FString InServerName) {
    this->MaxPlayers = InNumberOfPlayers;
    this->ServerName = InServerName;
}

void UCandyChaosLobbyGameInstance::PlayEnviromentMusic(USoundBase* Audio, float Volume, bool bIsPersistLevel)
{
    if (!IsValid(Audio)) return;

    this->Music = UGameplayStatics::CreateSound2D(GetWorld(), Audio, Volume, 1, 0, nullptr, bIsPersistLevel, true);

   // this->Music->Play();
}

void UCandyChaosLobbyGameInstance::StopEnviromentMusic()
{
    if (!IsValid(this->Music)) return;

    this->Music->Stop();
}

bool UCandyChaosLobbyGameInstance::GetIsFirstTimeLoading()
{
    return this->bIsFirstTimeLoading;
}

void UCandyChaosLobbyGameInstance::SetFirstTimeLoading(bool InbIsFirstTimeLoading)
{
    this->bIsFirstTimeLoading = InbIsFirstTimeLoading;
}

TSubclassOf<ARCharacterBase> UCandyChaosLobbyGameInstance::GetHeroeByName(FString InHeroeName)
{
    auto GetHeroeByName = [InHeroeName](const FHeroes& Heroe) {
        return Heroe.Name == InHeroeName;
        };

    auto InHeroeResource = this->Heroes.FindByPredicate(GetHeroeByName);

    return InHeroeResource->TargetClass;
}

void UCandyChaosLobbyGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCandyChaosLobbyGameInstance, MaxPlayers);
    DOREPLIFETIME(UCandyChaosLobbyGameInstance, ServerName);
}