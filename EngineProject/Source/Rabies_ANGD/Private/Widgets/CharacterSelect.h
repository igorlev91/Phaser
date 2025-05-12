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

	UFUNCTION()
	void SetSettingsState(bool state);

	UFUNCTION()
	void SetOwningCharacterController(ARCharacterSelectController* owningCharacterController);

	UFUNCTION()
	void PlayAudio();

private:
	ARCharacterSelectController* characterController;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* SettingsPanel;

	UPROPERTY(meta = (BindWidget))
	class ULobbyDisplay* LobbyDisplay;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* ToyBoxCheckBox;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* SparePartsCheckBox;

	UPROPERTY(meta = (BindWidget))
	class URButton* LeaveButton;

	UFUNCTION()
	void ToyBoxChecked(bool state);

	UFUNCTION()
	void SparePartsChecked(bool state);

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* EasyModeCheckBox;

	UFUNCTION()
	void EasyModeChecked(bool state);

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* MediumModeCheckBox;

	UFUNCTION()
	void MediumModeChecked(bool state);

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* HardModeCheckBox;

	UFUNCTION()
	void HardModeChecked(bool state);

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* LunaticModeCheckBox;

	UFUNCTION()
	void LunaticModeChecked(bool state);

	class ARMainMenuController* MenuController;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ClickAudio;

	UFUNCTION()
	void LeaveLobby();

protected:
	virtual void NativeConstruct() override;
};
