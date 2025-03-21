// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RCharacterSelectController.generated.h"

/**
 * 
 */
UCLASS()
class ARCharacterSelectController : public APlayerController
{
	GENERATED_BODY()

public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	virtual void BeginPlay() override;

private:
	void PostPossessionSetup(APawn* NewPawn);

	void CreateCharacterSelectUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCharacterSelect> CharacterSelectUIClass;

	UPROPERTY()
	UCharacterSelect* CharacterSelectUI;
};
