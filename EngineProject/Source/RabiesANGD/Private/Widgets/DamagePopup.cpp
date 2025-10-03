// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/DamagePopup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/RItemDataAsset.h"

void UDamagePopup::SetDamage(int damage)
{
	if (DamageText != nullptr)
	{
		if (damage > 0) //heal
		{
			FLinearColor NewColor = FLinearColor::Green; // Change to any color
			DamageText->SetColorAndOpacity(FSlateColor(NewColor));
		}
		else if (damage < 0) // damage
		{
			FLinearColor NewColor = FLinearColor::Red; // Change to any color
			DamageText->SetColorAndOpacity(FSlateColor(NewColor));
		}

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Setting Cost Text")));
		FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber(damage));
		DamageText->SetText(Text);
	}
}
