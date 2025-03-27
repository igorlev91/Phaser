// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayUI.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Widgets/PlayerItemInventory.h"

#include "Framework/RItemDataAsset.h"

#include "Widgets/PlayerAttributeGauge.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "Widgets/PlayerAbilityGauge.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Widgets/ChageBar.h"
#include "Components/ProgressBar.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "GameplayAbilitySpec.h"

#include "GameplayAbilities/RAttributeSet.h"
#include "Widgets/HealthBar.h"

void UGameplayUI::NativeConstruct()
{
	Super::NativeConstruct();

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetLevelAttribute()).AddUObject(this, &UGameplayUI::LevelUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetExpAttribute()).AddUObject(this, &UGameplayUI::ExpUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetNextLevelExpAttribute()).AddUObject(this, &UGameplayUI::NextLevelExpUpdated);

		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &UGameplayUI::HealthUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UGameplayUI::MaxHealthUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetScrapAttribute()).AddUObject(this, &UGameplayUI::ScrapUpdated);

		meleeStrength->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetMeleeAttackStrengthAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMeleeAttackStrengthAttribute()).AddUObject(meleeStrength, &UPlayerAttributeGauge::UpdateValue);
		meleeAttackSpeed->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetMeleeAttackCooldownReductionAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMeleeAttackCooldownReductionAttribute()).AddUObject(meleeAttackSpeed, &UPlayerAttributeGauge::UpdateValue);

		rangedStrength->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackStrengthAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetRangedAttackStrengthAttribute()).AddUObject(rangedStrength, &UPlayerAttributeGauge::UpdateValue);
		rangedAttackSpeed->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetRangedAttackCooldownReductionAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetRangedAttackCooldownReductionAttribute()).AddUObject(rangedAttackSpeed, &UPlayerAttributeGauge::UpdateValue);

		movementspeed->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetMovementSpeedAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMovementSpeedAttribute()).AddUObject(movementspeed, &UPlayerAttributeGauge::UpdateValue);

		damageReduction->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetDamageReductionAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetDamageReductionAttribute()).AddUObject(damageReduction, &UPlayerAttributeGauge::UpdateValue);

	}

	OwnerAbilitySystemComponent = OwnerASC;

	AbilityHorizontalBox->ClearChildren();
	const URAbilitySystemComponent* RAbilitySystemComp = Cast<URAbilitySystemComponent>(OwnerASC);
	if (RAbilitySystemComp)
	{
		TArray<const UGA_AbilityBase*> GrantedAbilities = RAbilitySystemComp->GetNonGenericAbilityCDOs();
		for (const UGA_AbilityBase* GrantedAbility : GrantedAbilities)
		{
			UPlayerAbilityGauge* newAbilityGague = CreateWidget<UPlayerAbilityGauge>(this, AbilityGaugeClass);
			UHorizontalBoxSlot* AbilitySlot = AbilityHorizontalBox->AddChildToHorizontalBox(newAbilityGague);
			newAbilityGague->SetupOwningAbilityCDO(GrantedAbility, OwnerASC);
			OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetRangedAttackCooldownReductionAttribute()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::CooldownUpdate);
			AbilitySlot->SetPadding(FMargin(5));
		}
	}

	float playerHealth = GetAttributeValue(URAttributeSet::GetHealthAttribute());
	float playerMaxHealth = GetAttributeValue(URAttributeSet::GetMaxHealthAttribute());
	int scrap = GetAttributeValue(URAttributeSet::GetScrapAttribute());

	PlayerHealth->SetHealth(playerHealth, playerMaxHealth);

	FText Text = FText::Format(FText::FromString("Scrap: {0}"), scrap);
	ScrapText->SetText(Text);

	//GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	//GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
}

void UGameplayUI::SetCrosshairState(bool state)
{
	if (Crosshair)
	{
		Crosshair->SetVisibility((state) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	crosshairState = state;
}

void UGameplayUI::SetTakeOffBarState(bool state, float charge)
{
	SuperJumpChargeBar->SetVisibility((state) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	SuperJumpChargeBar->Charge(charge);
}

void UGameplayUI::SetRevivalBarState(bool state, float charge)
{
	ReviveChargeBar->SetVisibility((state) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	ReviveChargeBar->Charge(charge);
}

void UGameplayUI::AddItem(URItemDataAsset* itemAsset)
{
	if (PlayerItemInventory)
	{
		PlayerItemInventory->AddItem(itemAsset);
	}
}

void UGameplayUI::LevelUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (LEvelText)
	{
		FText Text = FText::Format(FText::FromString("Lv {0}"), FText::AsNumber((int)ChangeData.NewValue));
		LEvelText->SetText(Text);
	}
}

void UGameplayUI::ExpUpdated(const FOnAttributeChangeData& ChangeData)
{
	float Percent = ChangeData.NewValue / GetAttributeValue(URAttributeSet::GetNextLevelExpAttribute());
	levelBar->SetPercent(Percent);
}

void UGameplayUI::NextLevelExpUpdated(const FOnAttributeChangeData& ChangeData)
{
}

void UGameplayUI::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealth->SetHealth(ChangeData.NewValue, GetAttributeValue(URAttributeSet::GetMaxHealthAttribute()));
}

void UGameplayUI::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealth->SetHealth(GetAttributeValue(URAttributeSet::GetHealthAttribute()), ChangeData.NewValue);
}

void UGameplayUI::ScrapUpdated(const FOnAttributeChangeData& ChangeData)
{
	FText Text = FText::Format(FText::FromString("Scrap: {0}"), FText::AsNumber((int)ChangeData.NewValue));
	ScrapText->SetText(Text);
}

float UGameplayUI::GetAttributeValue(const FGameplayAttribute& Attribute) const
{
	if (OwnerAbilitySystemComponent)
	{
		bool Found = false;
		float value = OwnerAbilitySystemComponent->GetGameplayAttributeValue(Attribute, Found);
		if (Found)
		{
			return value;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s Can't find attribute: %s "), *GetName(), *Attribute.AttributeName);

	return -1;
}