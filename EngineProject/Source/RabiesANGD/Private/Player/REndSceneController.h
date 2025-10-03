// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "REndSceneController.generated.h"

/**
 * 
 */
UCLASS()
class AREndSceneController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AREndSceneController();

public:
	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	virtual void BeginPlay() override;

private:

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void GetCameraView();

	UFUNCTION()
	void OnSkipCutscene();

	class ACineCameraActor* CineCamera;

	class ALevelSequenceActor* EndCutsceneSequence;

	void PostPossessionSetup(APawn* NewPawn);

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	void CreateUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class USkipCutsceneUI> SkipScreenUIClass;

	UPROPERTY()
	class USkipCutsceneUI* SkipScreenUI;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> MainMenu;

	FTimerHandle endCutsceneHandle;
};
