 // Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EndGameWidget.h"
#include "Components/TextBlock.h"

void UEndGameWidget::UpdateText(const FText& NewText)
{
	if (EndGameText)
	{
		EndGameText->SetText(NewText);
	}
}

void UEndGameWidget::UpdateTextColor(const FLinearColor& NewColor)
{
	if (EndGameText)
	{
		FSlateColor SlateColor(NewColor);
		EndGameText->SetColorAndOpacity(SlateColor);
	}
}
