// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BossHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Math/Color.h"
#include "Enemy/REnemyBase.h"
#include "Math/UnrealMathUtility.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Widgets/PlayerItemInventory.h"

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
#include "Widgets/GameplayUI.h"

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

void UBossHealthBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(FillColor);
}

void UBossHealthBar::SetHealth(float NewHealth, float MaxHealth)
{
	float Percent = NewHealth / MaxHealth;
	ProgressBar->SetPercent(Percent);
	ShaderBar->SetPercent(Percent);

	FText Text = FText::Format(FText::FromString("{0}/{1}"), FText::AsNumber((int)NewHealth), FText::AsNumber((int)MaxHealth));
	DisplayText->SetText(Text);
}

void UBossHealthBar::SetLevel(int newLevel)
{
	FText Text = FText::Format(FText::FromString("Lv {0}"), FText::AsNumber((int)newLevel));
	LevelText->SetText(Text);
}

void UBossHealthBar::Init(UGameplayUI* owner, int level, AREnemyBase* bossEnemy)
{
	BossEnemy = bossEnemy;
	OwnerUI = owner;
	SetLevel(level);
	NameText->SetText(FText::FromName(bossEnemy->bBossName));
	bossEnemy->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &UBossHealthBar::HealthUpdated);
	bossEnemy->OnDeadStatusChanged.AddUObject(this, &UBossHealthBar::DeadStatusUpdated);
}

void UBossHealthBar::DeadStatusUpdated(bool bIsDead)
{
	if (bIsDead)
	{
		OwnerUI->RemoveBossHealthFromUI(this);
	}
}

void UBossHealthBar::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	bool bFound = false;
	float value = BossEnemy->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMaxHealthAttribute(), bFound);
	if (bFound)
	{
		SetHealth(ChangeData.NewValue, value);
	}
}
