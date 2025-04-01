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
	
private:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* characterName;

	UPROPERTY(meta = (BindWidget))
	class UImage* characterIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* meleeIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* meleeText;

	UPROPERTY(meta = (BindWidget))
	class UImage* rangedIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* rangedText;

	UPROPERTY(meta = (BindWidget))
	class UImage* specialIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* specialText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ultimateIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ultimateText;

	UPROPERTY(meta = (BindWidget))
	class UImage* passiveIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* passiveText;
};
