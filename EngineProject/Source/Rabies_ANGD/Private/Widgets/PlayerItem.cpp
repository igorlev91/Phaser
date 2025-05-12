// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerItem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Framework/RItemDataAsset.h"
#include "Components/Image.h"

void UPlayerItem::SetItem(URItemDataAsset* data)
{
	if (data == nullptr)
		return;

	itemData = data;

	amount++;

	FSlateBrush brush;
	brush.SetResourceObject(data->ItemIcon);
	itemIcon->SetBrush(brush);

	if (data->ItemName == "KeyCard")
	{
		ItemSizeBox->SetWidthOverride(120.0f);
		ItemSizeBox->SetHeightOverride(120.0f);

		FText Text = FText::FromString("");
		itemAmount->SetText(Text);
	}
	else
	{
		FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber(amount));
		itemAmount->SetText(Text);
	}
}
