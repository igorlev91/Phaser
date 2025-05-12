// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ChestInteractUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UChestInteractUI::SetCostText(int costAmount)
{
	if (CostText != nullptr)
	{
		FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber(costAmount));
		CostText->SetText(Text);

	}
}
