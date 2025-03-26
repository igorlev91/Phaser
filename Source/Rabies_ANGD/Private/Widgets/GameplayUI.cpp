// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayUI.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"

#include "Widgets/ChageBar.h"
#include "Components/ProgressBar.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayAbilitySpec.h"

#include "GameplayAbilities/RAttributeSet.h"
#include "Widgets/HealthBar.h"

void UGameplayUI::NativeConstruct()
{
	Super::NativeConstruct();

	//quitButton->OnClicked.AddDynamic(this, &UGameplayUI::Quit);

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &UGameplayUI::HealthUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UGameplayUI::MaxHealthUpdated);
	}

	OwnerAbilitySystemComponent = OwnerASC;

	float playerHealth = GetAttributeValue(URAttributeSet::GetHealthAttribute());
	float playerMaxHealth = GetAttributeValue(URAttributeSet::GetMaxHealthAttribute());

	PlayerHealth->SetHealth(playerHealth, playerMaxHealth);

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

void UGameplayUI::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealth->SetHealth(ChangeData.NewValue, GetAttributeValue(URAttributeSet::GetMaxHealthAttribute()));
}

void UGameplayUI::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	PlayerHealth->SetHealth(GetAttributeValue(URAttributeSet::GetHealthAttribute()), ChangeData.NewValue);
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

//void UGameplayUI::Quit()
//{
//	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
//	if (PlayerController)
//	{
//		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
//	}
//}
