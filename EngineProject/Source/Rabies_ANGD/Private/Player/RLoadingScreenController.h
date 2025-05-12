// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RLoadingScreenController.generated.h"

/**
 * 
 */
UCLASS()
class ARLoadingScreenController : public APlayerController
{
	GENERATED_BODY()

public:
	ARLoadingScreenController();

public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	virtual void BeginPlay() override;
	
private:

	void PostPossessionSetup(APawn* NewPawn);

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	void CreateUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULoadingScreenUI> LoadingScreenUIClass;

	UPROPERTY()
	class ULoadingScreenUI* LoadingScreenUI;
};
