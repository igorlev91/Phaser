// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayEffectTypes.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameplayUI.generated.h"

/**
 * 
 */
UCLASS()
class UGameplayUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//this is the begin play or start
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetCrosshairState(bool state);

	UFUNCTION()
	void SetTakeOffBarState(bool state, float charge);

private:
	//************************//
	//        Health         //
	//************************//

	UPROPERTY(meta = (BindWidget))
	class UHealthBar* PlayerHealth;

	UPROPERTY(meta = (BindWidget))
	class UImage* Crosshair;

	bool crosshairState;

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxHealthUpdated(const FOnAttributeChangeData& ChangeData);

	float GetAttributeValue(const FGameplayAttribute& Attribute) const;

	UPROPERTY()
	const class UAbilitySystemComponent* OwnerAbilitySystemComponent;

	//************************//
	//        Abilites         //
	//************************//

	UPROPERTY(meta = (BindWidget))
	class UChageBar* SuperJumpChargeBar;

	//************************//
	//     Test n' Online      //
	//************************//

	//UPROPERTY(meta = (BindWidget))
	//class UButton* quitButton;

	//UFUNCTION()
	//void Quit();
};
