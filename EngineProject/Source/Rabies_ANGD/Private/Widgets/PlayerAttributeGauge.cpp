// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerAttributeGauge.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerAttributeGauge::UpdateValue(const FOnAttributeChangeData& ChangeData)
{
	if (bPercentage)
	{
		float percentage = (1.0f - (float)ChangeData.NewValue) * 100.0f;
		int32 finalValue = FMath::RoundToInt(percentage);
		ValueText->SetText(FText::Format(FText::FromString("{0}%"), FText::AsNumber(finalValue)));
	}
	else
	{
		ValueText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber((int)ChangeData.NewValue)));
	}
}

void UPlayerAttributeGauge::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IconTexture)
	{
		IconImage->SetBrushFromTexture(IconTexture);
	}
}

void UPlayerAttributeGauge::SetDefaultValue(float value)
{
	if (bPercentage)
	{
		float percentage = (1.0f - value) * 100.0f;
		int32 finalValue = FMath::RoundToInt(percentage);
		ValueText->SetText(FText::Format(FText::FromString("{0}%"), FText::AsNumber(finalValue)));
	}
	else
	{
		ValueText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(value)));
	}
}
