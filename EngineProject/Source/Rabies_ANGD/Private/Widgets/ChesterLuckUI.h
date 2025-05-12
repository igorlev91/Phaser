// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChesterLuckUI.generated.h"

/**
 * 
 */
UCLASS()
class UChesterLuckUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	void Charge(float charge);

	void Init(bool feelinLucky);

	UFUNCTION()
	bool IsFeelinLucky();

	UFUNCTION()
	void SetHealthUI(int storedHealth);

private:
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor FillColor;

	UPROPERTY()
	float CurrentCharge;

	UPROPERTY(EditAnywhere, Category = "Visual")
	float MaxChargeValue;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UProgressBar* ChargeBar;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* HealthStoredText;
};
