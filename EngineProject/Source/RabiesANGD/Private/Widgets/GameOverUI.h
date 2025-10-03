// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUI.generated.h"

/**
 * 
 */
UCLASS()
class UGameOverUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void ShowGameOverBlur();

protected:
    UPROPERTY(meta = (BindWidget))
    class UBackgroundBlur* BackgroundBlur;

    UPROPERTY(meta = (BindWidget))
    class UOverlay* PanelOverlay;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class URButton* QuitButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class URButton* LeaveButton;

    UFUNCTION()
    void OnQuitClick();

    UFUNCTION()
    void OnLeaveClick();

private:
    float CurrentBlurStrength;
    float TargetBlurStrength;
    float BlurLerpSpeed;

    FTimerHandle BlurUpdateTimer;

    void UpdateBlurStrength();
};
