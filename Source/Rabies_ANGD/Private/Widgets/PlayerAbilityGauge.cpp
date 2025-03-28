// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerAbilityGauge.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameplayAbilities/RAbilitySystemBlueprintLibrary.h"

void UPlayerAbilityGauge::SetupOwningAbilityCDO(const UGA_AbilityBase* OwningAbilityCDO, UAbilitySystemComponent* OwnerASC)
{
	AbilityCDO = OwningAbilityCDO;
	MyOwnerASC = OwnerASC;
	if (AbilityCDO)
	{
		UTexture2D* iconTex = AbilityCDO->GetIconTexture();
		if (iconTex)
		{
			if(!IconMat)
				IconMat = IconImage->GetDynamicMaterial();
		
			IconMat->SetTextureParameterValue(IconTextureMaterialParamName, iconTex);
		}

		CooldownDuration = URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(AbilityCDO, OwnerASC);
		//UE_LOG(LogTemp, Error, TEXT("Setting up %f"), CooldownDuration);

		FNumberFormattingOptions formattingOptions;
		formattingOptions.MaximumFractionalDigits = 2;
		CooldownDurationText->SetText(FText::AsNumber(CooldownDuration, &formattingOptions));

		CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
		SetupAbilityDelegates();
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
	CooldownDuration = URAbilitySystemBlueprintLibrary::GetAbilityStaticCooldownDuration(AbilityCDO, MyOwnerASC);

	FNumberFormattingOptions formattingOptions;
	formattingOptions.MaximumFractionalDigits = 2;
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration, &formattingOptions));
}

void UPlayerAbilityGauge::AbilityCommited(UGameplayAbility* Ability)
{
	if (Ability->GetClass() == AbilityCDO->GetClass())
	{
		CooldownDuration = 0;
		CooldownTimeRemaining = 0;
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

		//UE_LOG(LogTemp, Error, TEXT("Ability: %s Commited durtion: %f, cooldown remaining %f"), *Ability->GetName(), CooldownDuration, CooldownTimeRemaining);
		GetWorld()->GetTimerManager().SetTimer(CooldownTickTimerHandle, this, &UPlayerAbilityGauge::TickCooldown, CooldownTickInterval, true);
		FTimerHandle cooldownFinishedhandle;
		GetWorld()->GetTimerManager().SetTimer(cooldownFinishedhandle, this, &UPlayerAbilityGauge::CooldownFinished, CooldownTimeRemaining, false);
		
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
}
