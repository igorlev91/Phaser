// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ChesterLuckUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UChesterLuckUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ChargeBar == nullptr)
		return;
	ChargeBar->SetFillColorAndOpacity(FillColor);
}

void UChesterLuckUI::Charge(float charge)
{
	if (charge == 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("Value Guage: %s is geting a max value of 0, cannot calcuate percent for progress bar"), *GetName());
		return;
	}

	CurrentCharge += charge;
	CurrentCharge = FMathf::Clamp(CurrentCharge, 0, MaxChargeValue);

	float percent = CurrentCharge / MaxChargeValue;
	ChargeBar->SetPercent(percent);
}

void UChesterLuckUI::Init(bool feelinLucky)
{
	SetVisibility((feelinLucky) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	MaxChargeValue = 3;
	ChargeBar->SetPercent(0);
}

bool UChesterLuckUI::IsFeelinLucky()
{
	return CurrentCharge >= MaxChargeValue;
}
