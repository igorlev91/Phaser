// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayEffectTypes.h"

#include "PlayerAbilityGauge.generated.h"

struct FGameplayAbilitySpec;
class UGA_AbilityBase;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class UPlayerAbilityGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupOwningAbilityCDO(const UGA_AbilityBase* OwningAbilityCDO, class UAbilitySystemComponent* OwnerASC);
	void SetupAbilityDelegates();
	void CooldownUpdate(const FOnAttributeChangeData& ChangeData);

private:
	void AbilityCommited(UGameplayAbility* Ability);

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownDurationText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownCounterText;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownPercentMaterialParamName = "Percent";

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName IconTextureMaterialParamName = "Icon";

	const UGA_AbilityBase* AbilityCDO;

	class UAbilitySystemComponent* MyOwnerASC;

	UMaterialInstanceDynamic* IconMat;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownTickInterval = 1.0f / 24.0f;

	FTimerHandle CooldownTickTimerHandle;

	void TickCooldown();

	void CooldownFinished();

	float CooldownDuration = 0;

	float CooldownTimeRemaining = 0;
};
