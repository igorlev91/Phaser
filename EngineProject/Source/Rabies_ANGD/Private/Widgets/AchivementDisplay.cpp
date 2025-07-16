// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AchivementDisplay.h"

#include "Widgets/VolumeBarSlider.h"
#include "Components/Slider.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

#include "Widgets/ConnectOnlineMenu.h"
#include "Widgets/RButton.h"

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

void UAchivementDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	InfoPage->SetVisibility(ESlateVisibility::Collapsed);

	USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
	URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);

	if (LoadedGame)
	{
		if (LoadedGame->bChesterChallenge == true && characterString == "Chester")
		{
			CheckmarkIcon->SetVisibility(ESlateVisibility::Visible);
		}
		if (LoadedGame->bToniChallenge == true && characterString == "Toni")
		{
			CheckmarkIcon->SetVisibility(ESlateVisibility::Visible);
		}
		if (LoadedGame->bTexChallenge == true && characterString == "Tex")
		{
			CheckmarkIcon->SetVisibility(ESlateVisibility::Visible);
		}
		if (LoadedGame->bDotChallenge == true && characterString == "Dot")
		{
			CheckmarkIcon->SetVisibility(ESlateVisibility::Visible);
		}
		if (LoadedGame->bSecretChallenge == true && characterString == "Secret")
		{
			CheckmarkIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UAchivementDisplay::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	InfoPage->SetVisibility(ESlateVisibility::Visible);
}

void UAchivementDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	InfoPage->SetVisibility(ESlateVisibility::Collapsed);
}