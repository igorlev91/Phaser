// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Character/RLobbyCharacter.h"
#include "InGamePlayerInfo.generated.h"

USTRUCT(BlueprintType)
struct FInGamePlayerInfo
{
    GENERATED_BODY()

public:
    FInGamePlayerInfo();
    ~FInGamePlayerInfo();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame Player Info")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame Player Info")
    TSubclassOf<ARLobbyCharacter> HeroeSelected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InGame Player Info")
    int32 PlayerIndex;
};