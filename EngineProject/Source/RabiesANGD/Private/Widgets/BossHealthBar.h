// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayEffectTypes.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "BossHealthBar.generated.h"

class URAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UBossHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	void SetHealth(float NewHealth, float MaxHealth);

	void SetLevel(int newLevel);

	void Init(class UGameplayUI* owner, int level, class AREnemyBase* bossEnemy);

private:

	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);

	class AREnemyBase* BossEnemy;

	class UGameplayUI* OwnerUI;

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor FillColor;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UProgressBar* ProgressBar;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UProgressBar* ShaderBar;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* DisplayText;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* LevelText;

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* NameText;
};
