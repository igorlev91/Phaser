// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PingUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/RItemDataAsset.h"

void UPingUI::SetIcons(UTexture* imageIcon, FText text)
{
	if (CostText != nullptr)
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Setting Cost Text")));
		FText Text = text;// FText::Format(FText::FromString("Cost: {0}"), FText::AsNumber(costAmount));
		CostText->SetText(Text);

		FSlateBrush brush;
		brush.SetResourceObject((imageIcon == nullptr) ? nullptr : imageIcon);
		brush.ImageSize = FDeprecateSlateVector2D(65, 52);
		ItemIcon->SetBrush(brush);
	}
}