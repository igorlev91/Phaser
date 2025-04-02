#pragma once

#include "../../Character/RCharacterBase.h"
#include "LobbyPlayerInfo.generated.h"

USTRUCT(BlueprintType)
struct FLobbyPlayerInfo
{
    GENERATED_BODY()

public:
    FLobbyPlayerInfo();
    ~FLobbyPlayerInfo();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
    bool bPlayerReadyState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
    TSubclassOf<ARCharacterBase> HeroeSelected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
    int32 PlayerIndex;
};