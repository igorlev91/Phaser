// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ChageBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UChageBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ChargeBar == nullptr)
		return;
	ChargeBar->SetFillColorAndOpacity(FillColor);
}

void UChageBar::Charge(float charge)
{
	if (charge == 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("Value Guage: %s is geting a max value of 0, cannot calcuate percent for progress bar"), *GetName());
		return;
	}

	float percent = charge / MaxChargeValue;
	ChargeBar->SetPercent(percent);
}
