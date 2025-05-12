// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerItemInventory.generated.h"


class URItemDataAsset;
/**
 * 
 */
UCLASS()
class UPlayerItemInventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Player Item")
    void AddItem(class URItemDataAsset* itemAsset);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Player Item")
    TSubclassOf<class UPlayerItem> playerItemClass;

    UPROPERTY(meta = (BindWidget))
    class USizeBox* InventoryKeyCard;

    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* ItemContainer;

    UPROPERTY()
    TArray<class UPlayerItem*> AllItems;
};
