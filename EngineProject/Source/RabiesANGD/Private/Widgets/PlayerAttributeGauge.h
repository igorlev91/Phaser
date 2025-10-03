// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GameplayEffectTypes.h"

#include "PlayerAttributeGauge.generated.h"

/**
 * 
 */
UCLASS()
class UPlayerAttributeGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateValue(const FOnAttributeChangeData& ChangeData);
	virtual void NativePreConstruct() override;
	void SetDefaultValue(float value);

private:
	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bPercentage;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ValueText;
};
