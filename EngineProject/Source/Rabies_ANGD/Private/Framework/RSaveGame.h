// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class URSaveGame : public USaveGame
{
	GENERATED_BODY()
	

public:
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float BackgroundVolume = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float SFXVolume = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float VoicesVolume = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float Sensitivity = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	bool bItemSuggestions = false;
};
