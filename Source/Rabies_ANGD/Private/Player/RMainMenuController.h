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
	void ChangeOnlineMenuState(bool state);

	UFUNCTION()
	void ChangeMainMenuState(bool state);

	UFUNCTION()
	void ConfirmCharacterChoice();


private:
	class ACineCameraActor* CineCamera;

	class ALevelSequenceActor* MainMenuSequence;

	void PostPossessionSetup(APawn* NewPawn);

	void JoinedSession();

	void CreateMenuUI();

	UFUNCTION()
	void OnSequenceEnd();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainMenu> MenuUIClass;

	UPROPERTY()
	class UMainMenu* MenuUI;

	UPROPERTY()
	class URCharacterDefination* CurrentlyHoveredCharacter;

	UPROPERTY()
	class AEOSGameState* GameState;

	int myPlayerID;
};
