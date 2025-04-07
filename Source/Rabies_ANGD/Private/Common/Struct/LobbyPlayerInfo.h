#pragma once

#include "../../Character/RLobbyCharacter.h"
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
    TSubclassOf<ARLobbyCharacter> HeroeSelected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
    int32 PlayerIndex;
};