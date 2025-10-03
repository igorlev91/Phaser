// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SelectAbilityIcon.h"
#include "Framework/RCharacterDefination.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/CanvasPanelSlot.h"
#include "Framework/RCharacterSelectMode.h"
#include "Components/SizeBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "IMediaModule.h"
#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "Components/Image.h"
#include "Engine/Engine.h"

void USelectAbilityIcon::SetIcon(UTexture* textureToApply, UFileMediaSource* mediaSource)
{
	FSlateBrush brush;
	brush.SetResourceObject(textureToApply);
	brush.ImageSize = 120.f;
	thisIcon->SetBrush(brush);

	myMediaSource = mediaSource;

	//UMediaPlayer* MediaPlayer = LoadObject<UMediaPlayer>(nullptr, TEXT("/Game/Media/MyMediaPlayer.MyMediaPlayer"));
}

void USelectAbilityIcon::PlayMoive()
{
	if (myMediaSource)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s Broad casting media"), *GetName());
		OnMediaPlayerHovered.Broadcast(myMediaSource);
		/*ARCharacterSelectMode* GameMode = Cast<ARCharacterSelectMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit Media: %s"), *myMediaSource->GetName()));
			GameMode->PlayMediaFile(myMediaSource);
		}*/
	}
}

void USelectAbilityIcon::StopMovie()
{
	OnMediaPlayerLeft.Broadcast();
	/*ARCharacterSelectMode* GameMode = Cast<ARCharacterSelectMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->StopMediaFile();
	}*/
}

void USelectAbilityIcon::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent); //
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("yah")));
}

void USelectAbilityIcon::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

}