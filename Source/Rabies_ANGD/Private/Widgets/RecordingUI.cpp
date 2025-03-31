// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RecordingUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanelSlot.h"


void URecordingUI::LerpWarningImage(FSlateBrush slate)
{
    if (ErrorImage != nullptr)
    {
        if (ErrorImage->Brush.ImageSize.Y >= 120)
            return;

        slate.ImageSize = slate.ImageSize + 20.0f;
        ErrorImage->SetBrush(slate);
        MoveTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &URecordingUI::LerpWarningImage, slate));
    }
}

void URecordingUI::DisplayError()
{
	RecordingOverlay->SetRenderOpacity(0.0f);
	ErrorOverlay->SetRenderOpacity(1.0f);
    if (ErrorImage == nullptr)
        return;

	MoveTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &URecordingUI::LerpWarningImage, ErrorImage->Brush));
}
