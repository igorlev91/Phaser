// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChageBar.generated.h"

/**
 * 
 */
UCLASS()
class UChageBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	void Charge(float charge);

private:
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor FillColor;

	UPROPERTY(EditAnywhere, Category = "Visual")
	float MaxChargeValue;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UProgressBar* ChargeBar;
};
