// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class UPauseUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ReturnButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MenuButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnQuitClick();

	UFUNCTION()
	void OnReturnClick();

	UFUNCTION()
	void OnMenuClick();

};
