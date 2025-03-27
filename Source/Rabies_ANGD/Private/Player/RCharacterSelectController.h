// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RCharacterSelectController.generated.h"

class AEOSPlayerState;
/**
 * 
 */
UCLASS()
class ARCharacterSelectController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRep_PlayerState"))
	void BP_OnRep_PlayerState();

public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ConfirmCharacterChoice();

	int GetPlayerID();

private:
	class ACineCameraActor* CineCamera;

	class ALevelSequenceActor* MainMenuSequence;

	void PostPossessionSetup(APawn* NewPawn);

	void CreateCharacterSelectUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCharacterSelect> CharacterSelectUIClass;

	UPROPERTY()
	class URCharacterDefination* CurrentlyHoveredCharacter;

	UFUNCTION()
	void OnSequenceEnd();

	UPROPERTY()
	class AEOSGameState* GameState;

	UPROPERTY()
	UCharacterSelect* CharacterSelectUI;

	int myPlayerID;
};
