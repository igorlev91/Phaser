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
	
public:
	UFUNCTION()
	void SetPlayerController(ARMainMenuController* menuController);

private:
	UPROPERTY(meta = (BindWidget))
	class ULobbyDisplay* LobbyDisplay;

	class ARMainMenuController* MenuController;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

protected:
	virtual void NativeConstruct() override;
};
