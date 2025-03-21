// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RButton.generated.h"

/**
 * 
 */
UCLASS()
class URButton : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;

private:
	UFUNCTION()
	void OnHoverButton();

	UFUNCTION()
	void OnUnhoverButton();

	class UCanvasPanelSlot* RabiesCanvasSlot;

public:

	UPROPERTY(EditAnywhere, Category = "Rabies_Style")
	FVector2D DefaultScale;

	UPROPERTY(EditAnywhere, Category = "Rabies_Style")
	float DefaultRotation;

	UPROPERTY(EditAnywhere, Category = "Rabies_Style")
	float HoverScale;

	UPROPERTY(EditAnywhere, Category = "Rabies_Style")
	float HoverRotation;

	UPROPERTY(EditAnywhere, Category = "Rabies_Style")
	FText ButtonsText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RabiesText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RabiesButton;

	UPROPERTY(EditAnywhere, Category = "Rabies_ColorStyle")
	FSlateColor NormalColor;

	UPROPERTY(EditAnywhere, Category = "Rabies_ColorStyle")
	FSlateColor HoveredColor;

	UPROPERTY(EditAnywhere, Category = "Rabies_ColorStyle")
	FSlateColor PressedColor;

	UPROPERTY(EditAnywhere, Category = "Rabies_ColorStyle")
	FSlateColor BorderColor;
};
