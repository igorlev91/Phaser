// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"

class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowUIChange, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSensChange, float);
/**
 * 
 */
UCLASS()
class UPauseUI : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnShowUIChange OnShowUIChange;

	FOnSensChange OnSensChange;
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class URButton* QuitButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class URButton* LeaveButton;

	UPROPERTY(meta = (BindWidget))
	class UCheckBox* ShowUICheckBox;

	UPROPERTY(meta = (BindWidget))
	class UVolumeBarSlider* MusicSlider;

	UPROPERTY(meta = (BindWidget))
	class UVolumeBarSlider* VFXSlider;

	UPROPERTY(meta = (BindWidget))
	class UVolumeBarSlider* VoicesSlider;


	UFUNCTION()
	void SensChange(float newValue);

	UPROPERTY(meta = (BindWidget))
	class USlider* SensitivitySlider;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnQuitClick();

	UFUNCTION()
	void OnLeaveClick();

	UFUNCTION()
	void OnShowUIToggle(bool state);

private:
	void ChangeSensSaveData(class URSaveGame* LoadedGame, float newValue);
};
