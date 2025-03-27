// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Math/Color.h"
#include "Math/UnrealMathUtility.h"
#include "GameplayAbilities/RAttributeSet.h"

void UHealthBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(FillColor);
}

void UHealthBar::SetHealth(float NewHealth, float MaxHealth)
{
	float Percent = NewHealth / MaxHealth;
	ProgressBar->SetPercent(Percent);
	ShaderBar->SetPercent(Percent);
	
	FText Text = FText::Format(FText::FromString("{0}/{1}"), FText::AsNumber((int)NewHealth), FText::AsNumber((int)MaxHealth));
	DisplayText->SetText(Text);
}

void UHealthBar::SetLevel(int newLevel)
{
	FLinearColor levelColor = colorGreen;
	if (newLevel <= 50.0f)
	{
		float alpha = (newLevel - 1.0f) / 49.0f;
		levelColor = FMath::Lerp(colorGreen, colorYellow, alpha);
	}
	else
	{
		float alpha = (newLevel - 50.0f) / 50.0f;
		levelColor = FMath::Lerp(colorYellow, colorRed, alpha);
	}
	
	FText Text = FText::Format(FText::FromString("Lv {0}"), FText::AsNumber((int)newLevel));
	LevelText->SetText(Text);
	LevelText->SetColorAndOpacity(FSlateColor(levelColor));
}
