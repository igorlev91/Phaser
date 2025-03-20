// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(FillColor);
}

void UHealthBar::SetHealth(float NewHealth, float MaxHealth)
{
	if (NewHealth == 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("Value Guage: %s is geting a max value of 0, cannot calcuate percent for progress bar"), *GetName());
		return;
	}

	float Percent = NewHealth / MaxHealth;
	ProgressBar->SetPercent(Percent);

	FText Text = FText::Format(FText::FromString("{0}/{1}"), FText::AsNumber((int)NewHealth), FText::AsNumber((int)MaxHealth));
	DisplayText->SetText(Text);
}
