// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Input/Reply.h"

#include "AchivementDisplay.generated.h"

/**
 * 
 */
UCLASS()
class UAchivementDisplay : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FString characterString;

	UPROPERTY(meta = (BindWidget))
	class UImage* AchivementIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* CheckmarkIcon;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* InfoPage;

};
