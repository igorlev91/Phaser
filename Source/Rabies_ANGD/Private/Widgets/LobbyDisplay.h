// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyDisplay.generated.h"

/**
 * 
 */
UCLASS()
class ULobbyDisplay : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyPlayerNumber;

	UPROPERTY(meta = (BindWidget))
	class UListView* LobbyPlayerList;

protected:
	virtual void NativeConstruct() override;

private:

	UPROPERTY()
	class AEOSGameState* GameState;

	UFUNCTION()
	void SessionNameReplicated(const FName& newSessionName);

	UFUNCTION()
	void RefreshPlayerList();

	FTimerHandle PlayerListUpdateHandle;
};
