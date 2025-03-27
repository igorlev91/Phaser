// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateText(const FText& NewText);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateTextColor(const FLinearColor& NewColor);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EndGameText;
	
};
