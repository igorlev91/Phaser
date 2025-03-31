// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayUI.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Widgets/PlayerItemInventory.h"
#include "Widgets/BossHealthBar.h"

#include "Framework/RItemDataAsset.h"

#include "Widgets/PlayerAttributeGauge.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "Widgets/PlayerAbilityGauge.h"

#include "Character/RCharacterBase.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Widgets/ChageBar.h"
#include "Components/ProgressBar.h"
#include "Enemy/REnemyBase.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/Slider.h"
#include "Components/RadialSlider.h"
#include "Kismet/KismetTextLibrary.h"

#include "Engine/PostProcessVolume.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBox.h"

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

		specialStrength->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetAbilityStrengthAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetAbilityStrengthAttribute()).AddUObject(specialStrength, &UPlayerAttributeGauge::UpdateValue);
		specialAttackSpeed->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetAbilityCooldownReductionAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetAbilityCooldownReductionAttribute()).AddUObject(specialAttackSpeed, &UPlayerAttributeGauge::UpdateValue);

		ultimateStrength->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetUltimateStrengthAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetUltimateStrengthAttribute()).AddUObject(ultimateStrength, &UPlayerAttributeGauge::UpdateValue);
		ultimateAttackSpeed->SetDefaultValue(OwnerASC->GetNumericAttributeBase(URAttributeSet::GetUltimateCooldownReductionAttribute()));
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetUltimateCooldownReductionAttribute()).AddUObject(ultimateAttackSpeed, &UPlayerAttributeGauge::UpdateValue);

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
		int whichAttribute = 0;
		for (const UGA_AbilityBase* GrantedAbility : GrantedAbilities)
		{
			UPlayerAbilityGauge* newAbilityGague = CreateWidget<UPlayerAbilityGauge>(this, AbilityGaugeClass);
			UHorizontalBoxSlot* AbilitySlot = AbilityHorizontalBox->AddChildToHorizontalBox(newAbilityGague);
			newAbilityGague->SetupOwningAbilityCDO(GrantedAbility, OwnerASC);
			switch (whichAttribute)
			{
				default:
					OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMeleeAttackCooldownReductionAttribute()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::CooldownUpdate);
					break;

				case 1:
					OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetRangedAttackCooldownReductionAttribute()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::CooldownUpdate);
					break;

				case 2:
					OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetAbilityCooldownReductionAttribute()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::CooldownUpdate);
					break;

				case 3:
					OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetUltimateCooldownReductionAttribute()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::CooldownUpdate);
					break;

			}
			whichAttribute++;
			AbilitySlot->SetPadding(FMargin(5));
		}
	}

	float playerHealth = GetAttributeValue(URAttributeSet::GetHealthAttribute());
	float playerMaxHealth = GetAttributeValue(URAttributeSet::GetMaxHealthAttribute());
	int scrap = GetAttributeValue(URAttributeSet::GetScrapAttribute());

	PlayerHealth->SetHealth(playerHealth, playerMaxHealth);

	FText Text = FText::Format(FText::FromString("Scrap: {0}"), scrap);
	ScrapText->SetText(Text);

	ARCharacterBase* ownerCharacter = Cast<ARCharacterBase>(GetOwningPlayerPawn());
	if (ownerCharacter)
	{
		ownerCharacter->OnDeadStatusChanged.AddUObject(this, &UGameplayUI::DeadStatusUpdated);
	}

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

void UGameplayUI::AddEnemyBossHealth(int level, AREnemyBase* bossEnemy)
{
	UBossHealthBar* enemyBoss = CreateWidget<UBossHealthBar>(this, BossHealthClass);
	enemyBoss->Init(this, level, bossEnemy);
	BossVerticalBox->AddChild(enemyBoss);
}

void UGameplayUI::RemoveBossHealthFromUI(UBossHealthBar* barToRemove)
{
	BossVerticalBox->RemoveChild(barToRemove);
	barToRemove = nullptr;
}


void UGameplayUI::DeadStatusUpdated(bool bIsDead)
{
	if (bIsDead)
	{
		for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
		{
			PostProcessVolume = *It;
		}

		DownTimeBox->SetVisibility(ESlateVisibility::Visible);
		CurrentDeathDuration = 1.0f;
		DownTimeSlider->SetValue(1.0f);
		DownTimeText->SetText(FText::FromString(""));
		DeathHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGameplayUI::DeadTimer, CurrentDeathDuration));
	}
	else
	{
		if (PostProcessVolume)
		{
			FVector4 freshValue = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
			PostProcessVolume->Settings.ColorSaturation = freshValue;
			PostProcessVolume->Settings.ColorContrast = freshValue;
			PostProcessVolume->Settings.VignetteIntensity = 0;
			PostProcessVolume->Settings.SceneFringeIntensity = 0;
		}

		GetWorld()->GetTimerManager().ClearTimer(DeathHandle);
		DownTimeBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameplayUI::DeadTimer(float timeRemaining)
{
	if (CurrentDeathDuration > 0)
	{
		float rateOfChange = (0.0005f * GetAttributeValue(URAttributeSet::GetDownSurvivalTimeAttribute()));
		float remainingTime = (CurrentDeathDuration / rateOfChange);
		CurrentDeathDuration -= rateOfChange;
		DownTimeSlider->SetValue(CurrentDeathDuration / 1.0f);
		DownTimeText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(FMath::CeilToInt(remainingTime / 60.0f))));

		if (PostProcessVolume)
		{
			float inverseValue = FMath::Abs(1.0f - (CurrentDeathDuration / 1.0f));
			FVector4 newSaturation = FVector4(CurrentDeathDuration, CurrentDeathDuration, CurrentDeathDuration, 1.0f);
			PostProcessVolume->Settings.ColorSaturation = newSaturation;
			FVector4 newContrast = FVector4(1.0f, 1.0f, 1.0f, FMath::Lerp(1.0f, 1.2f, inverseValue));
			PostProcessVolume->Settings.ColorContrast = newContrast;
			PostProcessVolume->Settings.VignetteIntensity = inverseValue;
			PostProcessVolume->Settings.SceneFringeIntensity = inverseValue * 5.0f;
		}

		DeathHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGameplayUI::DeadTimer, CurrentDeathDuration));

	}
	else
	{
		DownTimeText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(0)));
		DownTimeBox->SetVisibility(ESlateVisibility::Hidden);
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