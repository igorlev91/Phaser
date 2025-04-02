// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Widgets/MainMenu/CandyMainMenu.h"
#include "./Common/Struct/Heroes.h"
#include "./Common/Struct/ConfigurationMaps.h"
#include "./Common/Struct/LobbyHeroeSpot.h"
#include "Common/Struct/InGamePlayerInfo.h"

#include "CandyChaosLobbyGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class  UCandyChaosLobbyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    
    UCandyChaosLobbyGameInstance(const FObjectInitializer& ObjectInitializer);

    /* Main Menu */

    UPROPERTY(EditAnyWhere)
    TSubclassOf<UCandyMainMenu> MenuClass;

    UFUNCTION(BlueprintCallable)
    UCandyMainMenu* LoadMenu();

    UFUNCTION(BlueprintCallable)
    void SetHostGame(bool InbIsHostGameMenu);

    UFUNCTION(BlueprintCallable)
    void SetFindGames(bool InbIsFindGamesMenu);

    UFUNCTION(BlueprintCallable)
    void SetOptionsMenu(bool InbIsOptionsMenu);

    UFUNCTION(BlueprintCallable)
    void SetBackToMainMenu(bool InbIsBackToMainMenu);

    UFUNCTION(BlueprintCallable)
    bool GetHostGame();

    UFUNCTION(BlueprintCallable)
    bool GetFindGames();

    UFUNCTION(BlueprintCallable)
    bool GetOptionsMenu();

    UFUNCTION(BlueprintCallable)
    bool GetBackToMainMenu();

    /* End Main Menu */

    /* General Settings */

    UPROPERTY(Replicated)
    int32 MaxPlayers;
    UPROPERTY(Replicated)
    FString ServerName;

    UPROPERTY(EditAnyWhere)
    TArray<FHeroes> Heroes;
    TArray<FInGamePlayerInfo> InGamePlayersInfo;
    UPROPERTY(EditAnyWhere)
    UTexture2D* LoadingScreenImage;

    UPROPERTY(EditAnyWhere)
    TMap<FString, FConfigurationMaps> ConfigurationMaps;

    UPROPERTY(EditAnyWhere)
    TMap<int32, FLobbyHeroeSpot> ConfigurationLobbyHeroeSpot;

    UFUNCTION(BlueprintCallable)
    void SetHostSettings(int32 InNumberOfPlayers, FString InServerName);

    UFUNCTION(BlueprintCallable)
    void PlayEnviromentMusic(USoundBase* Audio, float Volume, bool bIsPersistLevel);

    UFUNCTION(BlueprintCallable)
    void StopEnviromentMusic();

    UFUNCTION(BlueprintCallable)
    bool GetIsFirstTimeLoading();

    UFUNCTION(BlueprintCallable)
    void SetFirstTimeLoading(bool InbIsFirstTimeLoading);

    UFUNCTION()
    TSubclassOf<ARCharacterBase> GetHeroeByName(FString InHeroeName);

    /* End General Settings */    

private:
    UPROPERTY()
    bool bIsHostGameMenu{ false };
    UPROPERTY()
    bool bIsFindGamesMenu{ false };
    UPROPERTY()
    bool bIsOptionsMenu{ false };
    UPROPERTY()
    bool bIsBackToMainMenu{ false };
    UPROPERTY()
    bool bIsFirstTimeLoading{ true };
    UPROPERTY()
    UAudioComponent* Music;

    class UCandyMainMenu* Menu;
};