// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RMainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class ARMainMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ChangeOnlineMenuState(bool state);

	UFUNCTION()
	void ChangeMainMenuState(bool state);

private:
	void PostPossessionSetup(APawn* NewPawn);

	void CreateMenuUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainMenu> MenuUIClass;

	UPROPERTY()
	UMainMenu* MenuUI;
};
