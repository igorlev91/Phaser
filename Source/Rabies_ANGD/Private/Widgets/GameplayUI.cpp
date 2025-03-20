// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayUI.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &UGameplayUI::HealthUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UGameplayUI::MaxHealthUpdated);
	}

	OwnerAbilitySystemComponent = OwnerASC;
}

void UGameplayUI::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealthBar->SetHealth(ChangeData.NewValue, GetAttributeValue(URAttributeSet::GetMaxHealthAttribute()));
}

void UGameplayUI::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealthBar->SetHealth(GetAttributeValue(URAttributeSet::GetHealthAttribute()), ChangeData.NewValue);
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
