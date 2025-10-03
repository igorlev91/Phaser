// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecordingUI.generated.h"

/**
 * 
 */
UCLASS()
class URecordingUI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UOverlay* RecordingOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* ErrorOverlay;

	UPROPERTY(meta = (BindWidget))
	class UImage* ErrorImage;

	FTimerHandle MoveTimerHandle;

	UFUNCTION()
	void LerpWarningImage(FSlateBrush slate);

public:
	UFUNCTION()
	void DisplayError();
};
