// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"


#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void ChangeMainMenuState(bool state);

	UFUNCTION()
	void ChangeConnectMenuState(bool state);

	UFUNCTION()
	void ChangeSettingsState(bool state);

	UFUNCTION()
	void ChangeCreditsState(bool state);

	UFUNCTION()
	void ChangeAchivementsState(bool state);

	UFUNCTION()
	void OpenLoadingScreen(int whichLoadingScreen);
	
protected:

	virtual void NativeConstruct() override;

	class UEOSGameInstance* GameInst;

private:

	UFUNCTION()
	void SensChange(float newValue);

	UPROPERTY(meta = (BindWidget))
	class USlider* SensitivitySlider;

	UFUNCTION()
	void ItemSuggestionsChange(bool state);

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* ItemSuggestionsCheckBox;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(meta = (BindWidget))
	class UConnectOnlineMenu* OnlineMenuLobby;

	UPROPERTY(meta = (BindWidget))
	class URButton* SingeplayerBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* QuitMainMenuBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* MultiplayerBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* SettingsBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* CreditsBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* AchivementsBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* ReturnCredits;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AchivementsEarnedText;

	UFUNCTION()
	void ReturnFromCredits();

	UPROPERTY(meta = (BindWidget))
	class URButton* ReturnSettings;

	UPROPERTY(meta = (BindWidget))
	class URButton* ReturnAchivements;

	UFUNCTION()
	void ReturnFromSettings();

	UFUNCTION()
	void QuitMainMenu();


	UFUNCTION()
	void ReturnFromAchivments();

	UFUNCTION()
	void HideLoadingScreens();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TitleScreenText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* MainMenuOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* SettingsOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* CreditsOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* LoadingSessionOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* AuthorizingUserOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* AchivementsOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* AuthorizingUserOverlaySuccess;

	UFUNCTION()
	void SingleplayerClicked();

	UFUNCTION()
	void MultiplayerClicked();

	UFUNCTION()
	void SettingsClicked();

	UFUNCTION()
	void AchivementsClicked();

	UFUNCTION()
	void CreditsClicked();

	/*		Audio		*/
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* HoverAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ClickAudio;

	UFUNCTION()
	void PlayHoverAudio();

	UFUNCTION()
	void LoadGameLevel();
};
