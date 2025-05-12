// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterSelect.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CheckBox.h"
#include "Widgets/RButton.h"
#include "GameFramework/PlayerController.h"
#include "Player/RMainMenuController.h"

#include "Net/UnrealNetwork.h"
#include "Framework/EOSGameState.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/RCharacterDefination.h"
#include "Player/RCharacterSelectController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/EOSGameInstance.h"
#include "Sound/SoundCue.h"

void UCharacterSelect::LeaveLobby()
{
	FName levelName = FName(*FPackageName::ObjectPathToPackageName(MainMenuLevel.ToString()));
	UGameplayStatics::OpenLevel(GetWorld(), levelName);
}

void UCharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	if (ToyBoxCheckBox)
	{
		ToyBoxCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::ToyBoxChecked);
	}

	if (SparePartsCheckBox)
	{
		SparePartsCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::SparePartsChecked);
	}

	if (EasyModeCheckBox)
	{
		EasyModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::EasyModeChecked);
	}

	if (MediumModeCheckBox)
	{
		MediumModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::MediumModeChecked);
	}

	if (HardModeCheckBox)
	{
		HardModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::HardModeChecked);
	}

	if (LunaticModeCheckBox)
	{
		LunaticModeCheckBox->OnCheckStateChanged.AddDynamic(this, &UCharacterSelect::LunaticModeChecked);
	}

	if (LeaveButton)
	{
		LeaveButton->RabiesButton->OnClicked.AddDynamic(this, &UCharacterSelect::LeaveLobby);
	}
}

void UCharacterSelect::SetPlayerController(ARMainMenuController* menuController)
{
	MenuController = menuController;
}

void UCharacterSelect::SetSettingsState(bool state)
{
	SettingsPanel->SetRenderOpacity((state) ? 1.0f : 0.0f);
}

void UCharacterSelect::SetOwningCharacterController(ARCharacterSelectController* owningCharacterController)
{
	characterController = owningCharacterController;
}

void UCharacterSelect::PlayAudio()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);
}

void UCharacterSelect::ToyBoxChecked(bool state)
{
	AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
	if (owningPlayerState)
	{
		PlayAudio();
		owningPlayerState->SetToyBox(state);
		owningPlayerState->SeOwnsSettings(true);
	}
}

void UCharacterSelect::SparePartsChecked(bool state)
{
	AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
	if (owningPlayerState)
	{
		PlayAudio();
		owningPlayerState->SetSpareParts(state);
		owningPlayerState->SeOwnsSettings(true);
	}
}

void UCharacterSelect::EasyModeChecked(bool state)
{
	if (state)
	{
		AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
		if (owningPlayerState)
		{
			PlayAudio();
			owningPlayerState->SetSettings(ESettingsType::Easy);
			owningPlayerState->SeOwnsSettings(true);
		}

		MediumModeCheckBox->SetIsChecked(false);
		HardModeCheckBox->SetIsChecked(false);
		LunaticModeCheckBox->SetIsChecked(false);
	}
	else
	{
		PlayAudio();
	}
}

void UCharacterSelect::MediumModeChecked(bool state)
{
	if (state)
	{
		AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
		if (owningPlayerState)
		{
			PlayAudio();
			owningPlayerState->SetSettings(ESettingsType::Medium);
			owningPlayerState->SeOwnsSettings(true);
		}

		EasyModeCheckBox->SetIsChecked(false);
		HardModeCheckBox->SetIsChecked(false);
		LunaticModeCheckBox->SetIsChecked(false);
	}
	else
	{
		PlayAudio();
	}
}

void UCharacterSelect::HardModeChecked(bool state)
{
	if (state)
	{
		AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
		if (owningPlayerState)
		{
			PlayAudio();
			owningPlayerState->SetSettings(ESettingsType::Hard);
			owningPlayerState->SeOwnsSettings(true);
		}

		MediumModeCheckBox->SetIsChecked(false);
		EasyModeCheckBox->SetIsChecked(false);
		LunaticModeCheckBox->SetIsChecked(false);
	}
	else
	{
		PlayAudio();
	}
}

void UCharacterSelect::LunaticModeChecked(bool state)
{
	if (state)
	{
		AEOSPlayerState* owningPlayerState = Cast<AEOSPlayerState>(characterController->GetPawn()->GetPlayerState());
		if (owningPlayerState)
		{
			PlayAudio();
			owningPlayerState->SetSettings(ESettingsType::Lunitic);
			owningPlayerState->SeOwnsSettings(true);
		}

		MediumModeCheckBox->SetIsChecked(false);
		HardModeCheckBox->SetIsChecked(false);
		EasyModeCheckBox->SetIsChecked(false);
	}
	else
	{
		PlayAudio();
	}
}
