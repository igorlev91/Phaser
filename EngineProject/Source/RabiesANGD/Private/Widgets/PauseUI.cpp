// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PauseUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "Widgets/RButton.h"
#include "Components/CheckBox.h"
#include "Framework/RSaveGame.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"


void UPauseUI::SensChange(float newValue)
{
	SensitivitySlider->SetValue(newValue);

	if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
	{
		USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame)
		{
			ChangeSensSaveData(LoadedGame, newValue);
		}
	}
	else
	{
		URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
		if (NewSave)
		{
			ChangeSensSaveData(NewSave, newValue);
		}
	}
}

void UPauseUI::NativeConstruct()
{
	if (QuitButton)
	{
		QuitButton->RabiesButton->OnClicked.AddDynamic(this, &UPauseUI::OnQuitClick);
	}
	if (LeaveButton)
	{
		LeaveButton->RabiesButton->OnClicked.AddDynamic(this, &UPauseUI::OnLeaveClick);
	}
	if (ShowUICheckBox)
	{
		ShowUICheckBox->OnCheckStateChanged.AddDynamic(this, &UPauseUI::OnShowUIToggle);
	}

	USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
	URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
	float baseSens = 0.5f; // default volume
	if (LoadedGame)
	{
		baseSens = LoadedGame->Sensitivity;
		ChangeSensSaveData(LoadedGame, baseSens);
	}
	SensitivitySlider->OnValueChanged.AddDynamic(this, &UPauseUI::SensChange);
	SensitivitySlider->SetValue(baseSens);

}

void UPauseUI::OnQuitClick()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}
}

void UPauseUI::OnLeaveClick()
{
	UWorld* World = GetWorld();

	if (World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("RabiesMainMenu"));
	}
}

void UPauseUI::OnShowUIToggle(bool state)
{
	OnShowUIChange.Broadcast(state);
}

void UPauseUI::ChangeSensSaveData(URSaveGame* LoadedGame, float newValue)
{
	LoadedGame->Sensitivity = newValue;
	OnSensChange.Broadcast(newValue);
	UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
}
