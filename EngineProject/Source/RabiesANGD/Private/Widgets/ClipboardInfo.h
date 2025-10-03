// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClipboardInfo.generated.h"

/**
 * 
 */
UCLASS()
class UClipboardInfo : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SetCharacterInfo(class URCharacterDefination* Character);

	UFUNCTION()
	void PlayMoive(int index);

	UPROPERTY(meta = (BindWidget))
	class USelectAbilityIcon* meleeIcon;

	UPROPERTY(meta = (BindWidget))
	class USelectAbilityIcon* rangedIcon;

	UPROPERTY(meta = (BindWidget))
	class USelectAbilityIcon* specialIcon;

	UPROPERTY(meta = (BindWidget))
	class USelectAbilityIcon* ultimateIcon;

	UPROPERTY(meta = (BindWidget))
	class USelectAbilityIcon* passiveIcon;
	
private:

	void SetBestItem(class UImage* iconToChange, bool isBest, bool hideIt);

	void ShowStar(bool hasStar);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BestItemsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* characterName;

	UPROPERTY(meta = (BindWidget))
	class UImage* characterIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* meleeText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* rangedText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* specialText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ultimateText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* passiveText;

	UPROPERTY(meta = (BindWidget))
	class UImage* StarImage;


	UPROPERTY(meta = (BindWidget))
	class UImage* BigStickImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* SpareBatteriesImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* AmmoBoxImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* LaserSightImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* OldStopwatchImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* SpeedPillsImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* EnergyCellImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* ElectronicChargerImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* CannedFoodImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* IVBagImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* HardHatImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* UsedArmorImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* AntidoteImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* SprayBottleImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* BloodBagImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* FriendshipBraceletImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* TaserImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* BagOfNailsImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* RadioImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* FuelAndCoolantImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* HeaterImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* RodOfUraniumImage;

};
