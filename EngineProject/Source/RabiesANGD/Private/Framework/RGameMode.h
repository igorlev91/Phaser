// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RabiesGameModeBase.h"
#include "RGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARGameMode : public ARabiesGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* Controller) override;
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> DeathWidget;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> PauseWidget;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> InteractionWidget;

	UPROPERTY()
	UUserWidget* PauseUI = nullptr;

	UPROPERTY()
	UUserWidget* InteractUI = nullptr;

	UFUNCTION()
	void GameOver();

	UFUNCTION()
	void PausingGame(bool SetPause);

	UFUNCTION()
	void InteractingUI(bool SetInteraction);
};
