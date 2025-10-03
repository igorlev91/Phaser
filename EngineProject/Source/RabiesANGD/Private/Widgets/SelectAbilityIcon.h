// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectAbilityIcon.generated.h"

class UFileMediaSource;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMediaPlayerHovered, UFileMediaSource* /*bIsDead*/);
DECLARE_MULTICAST_DELEGATE(FOnMediaPlayerLeft /*bIsDead*/);

/**
 * 
 */
UCLASS()
class USelectAbilityIcon : public UUserWidget
{
	GENERATED_BODY()
	

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* thisIcon;

	UPROPERTY(VisibleAnywhere, Category = "Media")
	UFileMediaSource* myMediaSource;

public:

	FOnMediaPlayerHovered OnMediaPlayerHovered;

	FOnMediaPlayerLeft OnMediaPlayerLeft;

	void SetIcon(class UTexture* textureToApply, class UFileMediaSource* mediaSource);

	void PlayMoive();

	void StopMovie();

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
