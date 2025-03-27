// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerItemInventory.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Widgets/PlayerItem.h"
#include "Framework/RItemDataAsset.h"

void UPlayerItemInventory::AddItem(URItemDataAsset* itemAsset)
{
	UPlayerItem* playerItem = nullptr;

	UE_LOG(LogTemp, Error, TEXT("Adding item in invntory"));

	for (UPlayerItem* item : AllItems)
	{
		if (item->itemData->ItemName == itemAsset->ItemName)
		{
			playerItem = item;
			break;
		}
	}

	if (playerItem == nullptr)
	{
		playerItem = CreateWidget<UPlayerItem>(this, playerItemClass);
		ItemContainer->AddChildToHorizontalBox(playerItem);
		AllItems.Add(playerItem);
	}

	playerItem->SetItem(itemAsset);
}

void UPlayerItemInventory::NativeConstruct()
{
	Super::NativeConstruct();

}
