// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChestInteractUI.generated.h"

/**
 * 
 */
UCLASS()
class UChestInteractUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCostText(int costAmount);
	
private:

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* CostText;

	UPROPERTY(meta = (BindWidget))
	class UImage* InteractionButton;
};
