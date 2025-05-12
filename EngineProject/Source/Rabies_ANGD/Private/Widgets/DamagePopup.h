// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopup.generated.h"

/**
 * 
 */
UCLASS()
class UDamagePopup : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetDamage(int damage);
	
	
private:

	UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
	class UTextBlock* DamageText;
};
