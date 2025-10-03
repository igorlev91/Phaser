// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToyBoxMenu.generated.h"

class URItemDataAsset;
class AItemPickup;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDecision, AItemPickup*, URItemDataAsset*);
/**
 * 
 */
UCLASS()
class UToyBoxMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	FOnDecision OnDecision;

	//this is the begin play or start
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetUpItemButtons(AItemPickup* itemToChange, FString itemSelection);
	
private:

	AItemPickup* ChoosingItem;

	UFUNCTION()
	void SetButtonItem(class URButton* buttonEditting, URItemDataAsset* item);

	UFUNCTION()
	void ClickOption(URItemDataAsset* chosenItem);

	UFUNCTION()
	void HoveredOption(URItemDataAsset* chosenItem);


	UPROPERTY(meta = (BindWidget))
	class URButton* Item1;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item2;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item3;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item4;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item5;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item6;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item7;

	UPROPERTY(meta = (BindWidget))
	class URButton* Item8;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* itemTooltip;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* itemName;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> CommonLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> UncommonLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> RareLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* HoverAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ClickAudio;
};
