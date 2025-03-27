// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerItem.generated.h"

class URItemDataAsset;
/**
 * 
 */
UCLASS()
class UPlayerItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetItem(URItemDataAsset* data);
	
	UPROPERTY()
	URItemDataAsset* itemData;
private:
	UPROPERTY()
	int amount = 0;

	UPROPERTY(meta = (BindWidget))
	class UImage* itemIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* itemAmount;
};
