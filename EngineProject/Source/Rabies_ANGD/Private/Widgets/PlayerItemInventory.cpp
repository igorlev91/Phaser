// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerItemInventory.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Widgets/PlayerItem.h"
#include "Framework/RItemDataAsset.h"

void UPlayerItemInventory::AddItem(URItemDataAsset* itemAsset)
{
	UPlayerItem* playerItem = nullptr;

	for (UPlayerItem* item : AllItems)
	{
		if (item->itemData == nullptr)
			continue;

		if (itemAsset == nullptr)
			continue;

		if (item->itemData->ItemName == itemAsset->ItemName)
		{
			playerItem = item;
			playerItem->SetItem(itemAsset);
			return;
		}
	}

	if (playerItem == nullptr)
	{
		const int32 NumColumns = 3;

		int32 Index = AllItems.Num();
		int32 Row = Index / NumColumns;
		int32 Column = Index % NumColumns;

		UWidget* SlotWidget = ItemContainer->GetChildAt(Index);
		USizeBox* TargetSlot = Cast<USizeBox>(SlotWidget);

		playerItem = CreateWidget<UPlayerItem>(this, playerItemClass);
		if (playerItem)
		{
			if (itemAsset->ItemName == "KeyCard")
			{
				InventoryKeyCard->AddChild(playerItem);
				playerItem->SetItem(itemAsset);
				return;
			}

			if (TargetSlot)
			{
				TargetSlot->ClearChildren();
				TargetSlot->AddChild(playerItem);

				AllItems.Add(playerItem);
				playerItem->SetItem(itemAsset);
			}
		}
	}

}

void UPlayerItemInventory::NativeConstruct()
{
	Super::NativeConstruct();

}
