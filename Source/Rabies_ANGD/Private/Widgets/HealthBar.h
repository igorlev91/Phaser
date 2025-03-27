// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "HealthBar.generated.h"

class URAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	void SetHealth(float NewHealth, float MaxHealth);

	void SetLevel(int newLevel);

private:

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

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor colorGreen;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor colorYellow;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor colorRed;
};
