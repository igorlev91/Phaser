// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerAbilityGauge.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"



#include "Components/PanelSlot.h"
#include "Components/PanelWidget.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"

void UPlayerAbilityGauge::SetupOwningAbilityCDO(const UGA_AbilityBase* OwningAbilityCDO, UAbilitySystemComponent* OwnerASC, UTexture* inputButton)
{
	AbilityCDO = OwningAbilityCDO;
	MyOwnerASC = OwnerASC;
	if (AbilityCDO)
	{
		UTexture2D* iconTex = AbilityCDO->GetIconTexture();
		if (iconTex)
		{
			IconTexture = iconTex;

			if(!IconMat)
				IconMat = IconImage->GetDynamicMaterial();
		
			IconMat->SetTextureParameterValue(IconTextureMaterialParamName, iconTex);
		}

		UTexture2D* iconUltTex = AbilityCDO->GetUltimateIconTexture();
		if (iconUltTex)
		{
			UltimateIconTexture = iconUltTex;

			if (!IconMat)
				IconMat = IconImage->GetDynamicMaterial();

			IconMat->SetTextureParameterValue(IconTextureMaterialParamName, iconUltTex);
		}

		CooldownDuration = URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(AbilityCDO, OwnerASC);
		//UE_LOG(LogTemp, Error, TEXT("Setting up %f"), CooldownDuration);

		FSlateBrush brush;
		brush.SetResourceObject(inputButton);
		ButtonInputImage->SetBrush(brush);

		FNumberFormattingOptions formattingOptions;
		formattingOptions.MaximumFractionalDigits = 2;
		CooldownDurationText->SetText(FText::AsNumber(CooldownDuration, &formattingOptions));

		CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
		SetupAbilityDelegates();

		PaddingTickTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UPlayerAbilityGauge::NextTickPadding));
	}
}

void UPlayerAbilityGauge::SetupAbilityDelegates()
{
	UAbilitySystemComponent* OwningASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwningASC)
	{
		if (!OwningASC->AbilityCommittedCallbacks.IsBoundToObject(this))
		{
			OwningASC->AbilityCommittedCallbacks.AddUObject(this, &UPlayerAbilityGauge::AbilityCommited);
		}
	}
}

void UPlayerAbilityGauge::CooldownUpdate(const FOnAttributeChangeData& ChangeData)
{
	CooldownDuration = URAbilitySystemBlueprintLibrary::GetAbilityCooldownDuration(AbilityCDO, MyOwnerASC);

	FNumberFormattingOptions formattingOptions;
	formattingOptions.MaximumFractionalDigits = 2;
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration, &formattingOptions));
}

void UPlayerAbilityGauge::RefreshCooldownUpdate(const FGameplayEventData* EventData)
{
	float newTimeRemaining = URAbilitySystemBlueprintLibrary::GetAbilityCooldownRemaining(AbilityCDO, MyOwnerASC);

	CooldownTimeRemaining = newTimeRemaining;

	if (CooldownTimeRemaining <= 0)
	{
		CooldownFinished();
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(CooldownFinishedhandle);
	GetWorld()->GetTimerManager().SetTimer(CooldownFinishedhandle, this, &UPlayerAbilityGauge::CooldownFinished, CooldownTimeRemaining, false);
}

void UPlayerAbilityGauge::AbilityCommited(UGameplayAbility* Ability)
{
	if (Ability->GetClass() == AbilityCDO->GetClass())
	{
		if (IconTexture)
		{
			IconMat->SetTextureParameterValue(IconTextureMaterialParamName, IconTexture);
		}

		CooldownDuration = 0;
		CooldownTimeRemaining = 0;
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

		//UE_LOG(LogTemp, Error, TEXT("Ability: %s Commited durtion: %f, cooldown remaining %f"), *Ability->GetName(), CooldownDuration, CooldownTimeRemaining);
		GetWorld()->GetTimerManager().SetTimer(CooldownTickTimerHandle, this, &UPlayerAbilityGauge::TickCooldown, CooldownTickInterval, true);
		GetWorld()->GetTimerManager().SetTimer(CooldownFinishedhandle, this, &UPlayerAbilityGauge::CooldownFinished, CooldownTimeRemaining, false);
		
		CooldownCounterText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerAbilityGauge::TickCooldown()
{
	CooldownTimeRemaining -= CooldownTickInterval;
	float Percent = 1 - CooldownTimeRemaining / CooldownDuration;
	IconMat->SetScalarParameterValue(CooldownPercentMaterialParamName, Percent);

	FNumberFormattingOptions formattingOptions;
	formattingOptions.MaximumFractionalDigits = CooldownTimeRemaining > 1 ? 0 : 1;
	CooldownCounterText->SetText(FText::AsNumber(CooldownTimeRemaining, &formattingOptions));
}

void UPlayerAbilityGauge::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTickTimerHandle);
	IconMat->SetScalarParameterValue(CooldownPercentMaterialParamName, 1.0f);
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	if (UltimateIconTexture)
	{
		IconMat->SetTextureParameterValue(IconTextureMaterialParamName, UltimateIconTexture);
	}
}

void UPlayerAbilityGauge::NextTickPadding()
{
	if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(Slot))
	{
		HBoxSlot->SetPadding(FMargin(36.0f, 0.0f, 0.0f, 0.0f));
	}
}
