// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "Widgets/VolumeBarSlider.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"

#include "Widgets/ConnectOnlineMenu.h"
#include "Widgets/RButton.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/SoftObjectPtr.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/RSaveGame.h"
#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MultiplayerBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::MultiplayerClicked);
	SingeplayerBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::SingleplayerClicked);
	SettingsBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::SettingsClicked);
	CreditsBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::CreditsClicked);
	AchivementsBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::AchivementsClicked);

	ReturnCredits->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::ReturnFromCredits);
	ReturnSettings->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::ReturnFromSettings);
	ReturnAchivements->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::ReturnFromAchivments);

	QuitMainMenuBtn->RabiesButton->OnClicked.AddDynamic(this, &UMainMenu::QuitMainMenu);
	QuitMainMenuBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);

	MultiplayerBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	SingeplayerBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	SettingsBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	CreditsBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	AchivementsBtn->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	ReturnCredits->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	ReturnSettings->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);
	ReturnAchivements->RabiesButton->OnHovered.AddDynamic(this, &UMainMenu::PlayHoverAudio);

	GameInst = GetGameInstance<UEOSGameInstance>();

	USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
	URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
	
	if (ItemSuggestionsCheckBox)
	{
		ItemSuggestionsCheckBox->OnCheckStateChanged.AddDynamic(this, &UMainMenu::ItemSuggestionsChange);
		if (LoadedGame)
		{
			ItemSuggestionsCheckBox->SetIsChecked(LoadedGame->bItemSuggestions);
		}
	}

	float baseSens = 0.5f; // default volume
	if (LoadedGame)
	{
		baseSens = LoadedGame->Sensitivity;
	}
	SensitivitySlider->OnValueChanged.AddDynamic(this, &UMainMenu::SensChange);
	SensitivitySlider->SetValue(baseSens);

	//GameInst->SessionJoined.AddUObject(this, &UMainMenu::MoveToCharacterSelect);

	ChangeMainMenuState(true);
	ChangeConnectMenuState(false);
	ChangeSettingsState(false);
	ChangeCreditsState(false);
	ChangeAchivementsState(false);
}

void UMainMenu::SensChange(float newValue)
{
	SensitivitySlider->SetValue(newValue);

	if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
	{
		USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame)
		{
			LoadedGame->Sensitivity = newValue;

			UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
		}
	}
	else
	{
		URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
		if (NewSave)
		{
			NewSave->Sensitivity = newValue;

			UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
		}
	}
}

void UMainMenu::ItemSuggestionsChange(bool state)
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
	{
		USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame)
		{
			LoadedGame->bItemSuggestions = state;

			UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
		}
	}
	else
	{
		URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
		if (NewSave)
		{
			NewSave->bItemSuggestions = state;

			UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
		}
	}
}

void UMainMenu::ReturnFromCredits()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(true);
	ChangeCreditsState(false);
}

void UMainMenu::ReturnFromSettings()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(true);
	ChangeSettingsState(false);
}

void UMainMenu::QuitMainMenu()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}
}

void UMainMenu::ReturnFromAchivments()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(true);
	ChangeAchivementsState(false);
}

void UMainMenu::HideLoadingScreens()
{

	AuthorizingUserOverlay->SetVisibility(ESlateVisibility::Collapsed);
	AuthorizingUserOverlay->SetRenderOpacity(0.0f);

	AuthorizingUserOverlaySuccess->SetVisibility(ESlateVisibility::Collapsed);
	AuthorizingUserOverlaySuccess->SetRenderOpacity(0.0f);

	LoadingSessionOverlay->SetVisibility(ESlateVisibility::Collapsed);
	LoadingSessionOverlay->SetRenderOpacity(0.0f);
}

void UMainMenu::SingleplayerClicked()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMainMenu::LoadGameLevel, 0.2f, false);
}

void UMainMenu::MultiplayerClicked()
{
	if (GameInst)
	{
		UGameplayStatics::PlaySound2D(this, ClickAudio);

		OpenLoadingScreen(0);
		GameInst->Login();
	}
}

void UMainMenu::SettingsClicked()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(false);
	ChangeSettingsState(true);
}

void UMainMenu::AchivementsClicked()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(false);
	ChangeAchivementsState(true);
}

void UMainMenu::CreditsClicked()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	ChangeMainMenuState(false);
	ChangeCreditsState(true);
}

void UMainMenu::PlayHoverAudio()
{
	UGameplayStatics::PlaySound2D(this, HoverAudio);
}

void UMainMenu::LoadGameLevel()
{
	FName levelName = FName(*FPackageName::ObjectPathToPackageName(GameLevel.ToString()));
	UGameplayStatics::OpenLevel(GetWorld(), levelName);
}

void UMainMenu::ChangeMainMenuState(bool state)
{
	if (state)
	{
		HideLoadingScreens();
		MainMenuOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{

		MainMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::ChangeConnectMenuState(bool state)
{
	if (state)
	{
		HideLoadingScreens();
		OnlineMenuLobby->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{

		OnlineMenuLobby->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::ChangeSettingsState(bool state)
{
	if (state)
	{
		HideLoadingScreens();
		SettingsOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{

		SettingsOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::ChangeCreditsState(bool state)
{
	if (state)
	{
		HideLoadingScreens();
		CreditsOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{

		CreditsOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::ChangeAchivementsState(bool state)
{
	if (state)
	{
		HideLoadingScreens();
		AchivementsOverlay->SetVisibility(ESlateVisibility::Visible);

		int achivementsCompleted = 0;
		USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);

		if (LoadedGame)
		{
			if (LoadedGame->bChesterChallenge == true)
			{
				achivementsCompleted++;
			}
			if (LoadedGame->bToniChallenge == true)
			{
				achivementsCompleted++;
			}
			if (LoadedGame->bTexChallenge == true)
			{
				achivementsCompleted++;
			}
			if (LoadedGame->bDotChallenge == true)
			{
				achivementsCompleted++;
			}
			if (LoadedGame->bSecretChallenge == true)
			{
				achivementsCompleted++;
			}
		}

		FText Text = FText::Format(FText::FromString("Achievements Completed {0} / 5"), FText::AsNumber((int)achivementsCompleted));
		AchivementsEarnedText->SetText(Text);
	}
	else
	{

		AchivementsOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenu::OpenLoadingScreen(int whichLoadingScreen)
{
	HideLoadingScreens();
	MainMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
	OnlineMenuLobby->SetVisibility(ESlateVisibility::Collapsed);
	SettingsOverlay->SetVisibility(ESlateVisibility::Collapsed);
	CreditsOverlay->SetVisibility(ESlateVisibility::Collapsed);

	switch (whichLoadingScreen)
	{
	case 0:
		AuthorizingUserOverlay->SetRenderOpacity(1.0f);
		AuthorizingUserOverlay->SetVisibility(ESlateVisibility::Visible);
		break;

	case 1:
		AuthorizingUserOverlaySuccess->SetRenderOpacity(1.0f);
		AuthorizingUserOverlaySuccess->SetVisibility(ESlateVisibility::Visible);
		break;

	case 2:
		LoadingSessionOverlay->SetRenderOpacity(1.0f);
		LoadingSessionOverlay->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}
