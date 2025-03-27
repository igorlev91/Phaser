// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelect.generated.h"

/**
 * 
 */
UCLASS()
class UCharacterSelect : public UUserWidget
{
	GENERATED_BODY()
	

private:
	UPROPERTY(meta = (BindWidget))
	class ULobbyDisplay* LobbyDisplay;

	UPROPERTY(meta = (BindWidget))
	class URButton* ReadyUpButton;

	UFUNCTION()
	void ReadyUp();

	void LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

protected:
	virtual void NativeConstruct() override;
};
