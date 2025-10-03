// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PingUI.generated.h"

class URItemDataAsset;
/**
 * 
 */
UCLASS()
class UPingUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIcons(UTexture* imageIcon, FText text);

	void SetItemIcon(URItemDataAsset* data);
	
private:

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* CostText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* PingIcon;
};
