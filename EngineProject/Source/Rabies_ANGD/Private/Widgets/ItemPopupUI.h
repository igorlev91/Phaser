// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemPopupUI.generated.h"

class URItemDataAsset;
/**
 * 
 */
UCLASS()
class UItemPopupUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetItem(URItemDataAsset* data);

private:

	float opacity = 0;

	FTimerHandle TickTimerHandle;
	FTimerHandle HideTimerHandle;

	UFUNCTION()
	void TickOpacity(bool bShow);

	UFUNCTION()
	void SetHiddenState();

	UPROPERTY(meta = (BindWidget))
	class UImage* itemIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* itemDescription;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* itemTooltip;

protected:
	virtual void NativeConstruct() override;
};
