// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerItem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Framework/RItemDataAsset.h"
#include "Components/Image.h"

void UPlayerItem::SetItem(URItemDataAsset* data)
{
	itemData = data;

	amount++;

	FSlateBrush brush;
	brush.SetResourceObject(data->ItemIcon);
	itemIcon->SetBrush(brush);

	FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber(amount));
	itemAmount->SetText(Text);
}
