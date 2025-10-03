// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayUI.h"
#include "Actors/ItemPickup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Widgets/ChesterLuckUI.h"
#include "Widgets/ItemPopupUI.h"
#include "Widgets/PlayerItemInventory.h"
#include "Widgets/BossHealthBar.h"
#include "Player/RPlayerBase.h"
#include "Widgets/ToyBoxMenu.h"

#include "Framework/RItemDataAsset.h"
#include "Widgets/GameOverUI.h"

#include "Widgets/PlayerAttributeGauge.h"

#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "Widgets/PlayerAbilityGauge.h"

#include "Character/RCharacterBase.h"

#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Widgets/ChageBar.h"
#include "Components/ProgressBar.h"
#include "Enemy/REnemyBase.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Framework/RSaveGame.h"
#include "Framework/EOSGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/Slider.h"
#include "Components/RadialSlider.h"
#include "Kismet/KismetTextLibrary.h"

#include "Engine/PostProcessVolume.h"

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

		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetAirComboAttribute()).AddUObject(this, &UGameplayUI::AirComboUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetCritComboAttribute()).AddUObject(this, &UGameplayUI::CritComboUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetCritComboTimerAttribute()).AddUObject(this, &UGameplayUI::CritComboTimerUpdated);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealingDoneAttribute()).AddUObject(this, &UGameplayUI::HealingDoneUpdated);

		OwnerASC->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetPassiveHealingAttribute()).AddUObject(this, &UGameplayUI::ChesterPassiveUpdated);
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

	AbilityVerticalBox->ClearChildren();
	const URAbilitySystemComponent* RAbilitySystemComp = Cast<URAbilitySystemComponent>(OwnerASC);
	if (RAbilitySystemComp)
	{
		TArray<const UGA_AbilityBase*> GrantedAbilities = RAbilitySystemComp->GetNonGenericAbilityCDOs();
		int whichAttribute = 0;
		for (const UGA_AbilityBase* GrantedAbility : GrantedAbilities)
		{
			UPlayerAbilityGauge* newAbilityGague = CreateWidget<UPlayerAbilityGauge>(this, AbilityGaugeClass);
			OwnerASC->GenericGameplayEventCallbacks.FindOrAdd(URAbilityGenericTags::GetCooldownRefreshTag()).AddUObject(newAbilityGague, &UPlayerAbilityGauge::RefreshCooldownUpdate);

			UVerticalBoxSlot* AbilitySlot = AbilityVerticalBox->AddChildToVerticalBox(newAbilityGague);
			newAbilityGague->SetupOwningAbilityCDO(GrantedAbility, OwnerASC, ButtonInputs[whichAttribute]);
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
			AbilitySlot->SetPadding(FMargin(27));
		}
	}

	float playerHealth = GetAttributeValue(URAttributeSet::GetHealthAttribute());
	float playerMaxHealth = GetAttributeValue(URAttributeSet::GetMaxHealthAttribute());
	int scrap = GetAttributeValue(URAttributeSet::GetScrapAttribute());

	PlayerHealth->SetHealth(playerHealth, playerMaxHealth);
	PlayerHealth->SetPersonalView();

	FText Text = FText::Format(FText::FromString("{0}"), scrap);
	ScrapText->SetText(Text);

	ARCharacterBase* ownerCharacter = Cast<ARCharacterBase>(GetOwningPlayerPawn());
	if (ownerCharacter)
	{
		ownerCharacter->OnDeadStatusChanged.AddUObject(this, &UGameplayUI::DeadStatusUpdated);
	}

	OwnerPlayer = Cast<ARPlayerBase>(GetOwningPlayerPawn());
	if (OwnerPlayer)
	{
		ChesterLuckUI->Init(OwnerPlayer->bFeelinLucky);
	}

	if (ToyBoxUI)
	{
		ToyBoxUI->OnDecision.AddUObject(this, &UGameplayUI::CloseToyBox);
	}

	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		PostProcessVolume = *It;
	}
	if (PostProcessVolume)
	{
		colorContrast = PostProcessVolume->Settings.ColorContrast;
		colorSaturation = PostProcessVolume->Settings.ColorSaturation;
		sceneFringeIntensity = PostProcessVolume->Settings.SceneFringeIntensity;
		vignetteIntensity = PostProcessVolume->Settings.VignetteIntensity;

	}

	if (GameOverUI)
	{
		GameOverUI->SetVisibility(ESlateVisibility::Collapsed);// do collasped instead of hidden
	}

	TimerTickHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGameplayUI::TickTimer));
}

void UGameplayUI::TickTimer()
{

	if (TimerElaspedText == nullptr)
		return;

	timeElasped += GetWorld()->GetDeltaSeconds();

	int32 TotalSeconds = FMath::CeilToInt(timeElasped);
	int32 Hours = TotalSeconds / 3600;
	int32 Minutes = (TotalSeconds % 3600) / 60;
	int32 Seconds = TotalSeconds % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds);
	TimerElaspedText->SetText(FText::FromString(TimeString));

	TimerTickHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGameplayUI::TickTimer));

}

void UGameplayUI::OpenToyBox(AItemPickup* itemToChange, FString itemSelection)
{
	if (ToyBoxUI)
	{
		ToyBoxUI->SetVisibility(ESlateVisibility::Visible);
		ToyBoxUI->SetUpItemButtons(itemToChange, itemSelection);
	}
}

void UGameplayUI::CloseToyBox(AItemPickup* choosingItem, URItemDataAsset* chosenItem)
{
	if (ToyBoxUI)
	{
		//GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		//GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
		OnToyboxDecision.Broadcast(choosingItem, chosenItem);
		ToyBoxUI->SetVisibility(ESlateVisibility::Collapsed);
	}
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

void UGameplayUI::SetChargeSuperPunchState(bool state, float charge)
{
	SuperPunchChargeBar->SetVisibility((state) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	SuperPunchChargeBar->Charge(charge);
}

void UGameplayUI::AddItem(URItemDataAsset* itemAsset)
{
	if (PlayerItemInventory)
	{
		PlayerItemInventory->AddItem(itemAsset);
	}
	if (ItemPopupUI)
	{
		ItemPopupUI->SetItem(itemAsset);
	}
}

bool UGameplayUI::IsItemUnique(URItemDataAsset* itemAssetToCheck)
{
	if (PlayerItemInventory)
	{
		return PlayerItemInventory->IsItemUnique(itemAssetToCheck);
	}

	return false;
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

bool UGameplayUI::CashMyLuck()
{
	if (ChesterLuckUI->IsFeelinLucky())
	{
		ChesterLuckUI->Charge(-3);
		return true;
	}
	else
	{
		return false;
	}
}


void UGameplayUI::DeadStatusUpdated(bool bIsDead)
{
	if (bIsDead)
	{

		DownTimeBox->SetVisibility(ESlateVisibility::Visible);
		CurrentDeathDuration = 1.0f;
		DownTimeSlider->SetValue(1.0f);
		//UE_LOG(LogTemp, Warning, TEXT("dead"), *GetName());
		DownTimeText->SetText(FText::FromString(""));
		DeathHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGameplayUI::DeadTimer, CurrentDeathDuration));
	}
	else
	{
		if (PostProcessVolume)
		{
			PostProcessVolume->Settings.ColorSaturation = colorSaturation;
			PostProcessVolume->Settings.ColorContrast = colorContrast;
			PostProcessVolume->Settings.VignetteIntensity = vignetteIntensity;
			PostProcessVolume->Settings.SceneFringeIntensity = sceneFringeIntensity;
		}

		GetWorld()->GetTimerManager().ClearTimer(DeathHandle);
		DownTimeBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameplayUI::GameOver()
{
	if (GameOverUI)
	{
		GameOverUI->SetVisibility(ESlateVisibility::Visible);// do collasped instead of hidden
		GameOverUI->ShowGameOverBlur();
	}
}

void UGameplayUI::SetAirComboText(int kills)
{
	if (kills == 0)
	{
		FText Text = FText::Format(FText::FromString(""), FText::AsNumber((int)kills));
		AirComboText->SetText(Text);
		return;
	}

	FSlateFontInfo FontInfo = AirComboText->GetFont();
	FontInfo.Size = 3 * kills; // Set desired font size
	FontInfo.Size = FMath::Clamp(FontInfo.Size, 5, 60);

	AirComboText->SetFont(FontInfo);
	FText Text = FText::Format(FText::FromString("- Air Combo: {0} -"), FText::AsNumber((int)kills));
	AirComboText->SetText(Text);

	if (kills >= 20)
	{
		FLinearColor Color = FLinearColor::Green;
		AirComboText->SetColorAndOpacity(FSlateColor(Color));

		if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
		{
			USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
			URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
			if (LoadedGame)
			{
				LoadedGame->bDotChallenge = true;

				UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
			}
		}
		else
		{
			URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
			if (NewSave)
			{
				NewSave->bDotChallenge = true;

				UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
			}
		}
	}
}

void UGameplayUI::SetCritComboText(int kills, int timeRemaining)
{
	if (kills == 0 || timeRemaining <= 0)
	{
		FText Text = FText::Format(FText::FromString(""), FText::AsNumber((int)kills));
		CritComboText->SetText(Text);
		return;
	}

	FSlateFontInfo FontInfo = CritComboText->GetFont();
	FontInfo.Size = (2 * kills) + 15; // Set desired font size
	FontInfo.Size = FMath::Clamp(FontInfo.Size, 15, 45);

	CritComboText->SetFont(FontInfo);
	FText Text = FText::Format(FText::FromString("- Consecutive Crits: {0} TIME[{1}] -"), FText::AsNumber((int)kills), (int)timeRemaining);
	CritComboText->SetText(Text);

	if (kills >= 10 && timeRemaining >= 1)
	{
		FLinearColor Color = FLinearColor::Green;
		CritComboText->SetColorAndOpacity(FSlateColor(Color));

		if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
		{
			USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
			URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
			if (LoadedGame)
			{
				LoadedGame->bTexChallenge = true;

				UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
			}
		}
		else
		{
			URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
			if (NewSave)
			{
				NewSave->bTexChallenge = true;

				UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
			}
		}
	}
}

void UGameplayUI::SetHealingGivenText(int health)
{
	FText Text = FText::Format(FText::FromString("Healing Done: {0}"), FText::AsNumber((int)health));
	HealthGivenText->SetText(Text);

	if (health >= 1000)
	{
		if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
		{
			USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
			URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
			if (LoadedGame)
			{
				if (GetOwningPlayerPawn()->GetName().Contains("Chester"))
					LoadedGame->bChesterChallenge = true;

				UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
			}
		}
		else
		{
			URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
			if (NewSave)
			{
				if (GetOwningPlayerPawn()->GetName().Contains("Chester"))
					NewSave->bChesterChallenge = true;

				UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
			}
		}
	}
}

void UGameplayUI::DeadTimer(float timeRemaining)
{
	if (CurrentDeathDuration > 0)
	{
		float rateOfChange = (0.00045f * GetAttributeValue(URAttributeSet::GetDownSurvivalTimeAttribute()));
		float remainingTime = (CurrentDeathDuration / rateOfChange);
		CurrentDeathDuration -= rateOfChange;
		DownTimeSlider->SetValue(CurrentDeathDuration / 1.0f);
		DownTimeText->SetText(FText::Format(FText::FromString("{0}"), FText::AsNumber(FMath::CeilToInt(remainingTime / 60.0f))));

		if (PostProcessVolume)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Post processing"), *GetName());

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

		//fully dead
		OnFullyDead.Broadcast();
	}
}



void UGameplayUI::LevelUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (ChesterLuckUI)
	{
		ChesterLuckUI->Charge(ChangeData.NewValue - ChangeData.OldValue);
	}

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

void UGameplayUI::AirComboUpdated(const FOnAttributeChangeData& ChangeData)
{
	SetAirComboText(ChangeData.NewValue);
}

void UGameplayUI::CritComboUpdated(const FOnAttributeChangeData& ChangeData)
{
	SetCritComboText(ChangeData.NewValue, GetAttributeValue(URAttributeSet::GetCritComboTimerAttribute()));
}

void UGameplayUI::CritComboTimerUpdated(const FOnAttributeChangeData& ChangeData)
{
	SetCritComboText(GetAttributeValue(URAttributeSet::GetCritComboAttribute()), ChangeData.NewValue);
}

void UGameplayUI::HealingDoneUpdated(const FOnAttributeChangeData& ChangeData)
{
	SetHealingGivenText(ChangeData.NewValue);
}

void UGameplayUI::ChesterPassiveUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (ChesterLuckUI)
	{
		ChesterLuckUI->SetHealthUI(ChangeData.NewValue);
	}
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
	FText Text = FText::Format(FText::FromString("{0}"), FText::AsNumber((int)ChangeData.NewValue));
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