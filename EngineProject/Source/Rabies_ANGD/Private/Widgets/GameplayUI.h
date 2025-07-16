// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayEffectTypes.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameplayUI.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnToyboxDecision, AItemPickup*, URItemDataAsset*);
DECLARE_MULTICAST_DELEGATE(FOnFullyDead);
/**
 * 
 */
UCLASS()
class UGameplayUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnToyboxDecision OnToyboxDecision;

	FOnFullyDead OnFullyDead;

	//this is the begin play or start
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OpenToyBox(class AItemPickup* itemToChange, FString itemSelection);
	
	UFUNCTION()
	void CloseToyBox(AItemPickup* choosingItem, URItemDataAsset* chosenItem);

	UFUNCTION()
	void SetCrosshairState(bool state);

	UFUNCTION()
	void SetTakeOffBarState(bool state, float charge);

	UFUNCTION()
	void SetRevivalBarState(bool state, float charge);

	UFUNCTION()
	void SetChargeSuperPunchState(bool state, float charge);

	UFUNCTION()
	void AddItem(class URItemDataAsset* itemAsset);

	UFUNCTION()
	void AddEnemyBossHealth(int level, class AREnemyBase* bossEnemy);

	UFUNCTION()
	void RemoveBossHealthFromUI(class UBossHealthBar* barToRemove);

	UFUNCTION()
	bool CashMyLuck();

	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);


	UFUNCTION()
	void GameOver();

	float timeElasped;

	UFUNCTION()
	void SetAirComboText(int kills);

	UFUNCTION()
	void SetCritComboText(int kills, int timeRemaining);

	UFUNCTION()
	void SetHealingGivenText(int health);


private:

	class ARPlayerBase* OwnerPlayer;

	void TickTimer();

	FTimerHandle TimerTickHandle;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerElaspedText;


private:

	FVector colorSaturation;
	FVector colorContrast;
	float vignetteIntensity;
	float sceneFringeIntensity;

	//************************//
	//        Health         //
	//************************//


	UFUNCTION()
	void DeadTimer(float timeRemaining);

	UPROPERTY(meta = (BindWidget))
	class UToyBoxMenu* ToyBoxUI;

	UPROPERTY(meta = (BindWidget))
	class UGameOverUI* GameOverUI;

	UPROPERTY(meta = (BindWidget))
	class UItemPopupUI* ItemPopupUI;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* BossVerticalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<class UBossHealthBar> BossHealthClass;

	UPROPERTY()
	TArray<class UBossHealthBar*> BossHealthBars;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* DownTimeBox;

	UPROPERTY(meta = (BindWidget))
	class URadialSlider* DownTimeSlider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DownTimeText;

	class APostProcessVolume* PostProcessVolume;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* levelBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LEvelText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AirComboText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthGivenText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CritComboText;

	UPROPERTY(meta = (BindWidget))
	class UHealthBar* PlayerHealth;

	UPROPERTY(meta = (BindWidget))
	class UImage* Crosshair;

	UPROPERTY(meta = (BindWidget))
	class UImage* ScrapIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScrapText;

	UPROPERTY(meta = (BindWidget))
	class UPlayerItemInventory* PlayerItemInventory;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* AbilityVerticalBox;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* meleeStrength;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* meleeAttackSpeed;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* rangedStrength;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* rangedAttackSpeed;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* specialStrength;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* specialAttackSpeed;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* ultimateStrength;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* ultimateAttackSpeed;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* movementspeed;

	UPROPERTY(meta = (BindWidget))
	class UPlayerAttributeGauge* damageReduction;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class UPlayerAbilityGauge> AbilityGaugeClass;

	UPROPERTY(meta = (BindWidget))
	class UChesterLuckUI* ChesterLuckUI;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<UTexture*> ButtonInputs;

	bool crosshairState;

	void LevelUpdated(const FOnAttributeChangeData& ChangeData);
	void ExpUpdated(const FOnAttributeChangeData& ChangeData);
	void NextLevelExpUpdated(const FOnAttributeChangeData& ChangeData);

	void ChesterPassiveUpdated(const FOnAttributeChangeData& ChangeData);
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxHealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ScrapUpdated(const FOnAttributeChangeData& ChangeData);

	float GetAttributeValue(const FGameplayAttribute& Attribute) const;

	//void RefreshCooldowns(const struct FGameplayEventData* EventData);

	UPROPERTY()
	const class UAbilitySystemComponent* OwnerAbilitySystemComponent;

	//************************//
	//        Abilites         //
	//************************//

	float CurrentDeathDuration;

	FTimerHandle DeathHandle;

	UPROPERTY(meta = (BindWidget))
	class UChageBar* SuperJumpChargeBar;

	UPROPERTY(meta = (BindWidget))
	class UChageBar* ReviveChargeBar;

	UPROPERTY(meta = (BindWidget))
	class UChageBar* SuperPunchChargeBar;

	//************************//
	//     Test n' Online      //
	//************************//
};
