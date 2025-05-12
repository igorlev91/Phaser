// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RCharacterSelectController.generated.h"

class URCharacterDefination;
class AEOSPlayerState;
class UFileMediaSource;
/**
 * 
 */
UCLASS()
class ARCharacterSelectController : public APlayerController
{
	GENERATED_BODY()

public:
	ARCharacterSelectController();

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

	UFUNCTION()
	void SecretChoice();

	UFUNCTION()
	void NextCharacter(class ARRightButton* rightButton);

	UFUNCTION()
	void GetNextCharacterCage();

	UFUNCTION(Client, Reliable)
	void SetSettingsState(bool state);

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "RabiesMedia")
	void PlayMediaFile(UFileMediaSource* mediaSource);


	UFUNCTION(BlueprintImplementableEvent, Category = "RabiesMedia")
	void StopMediaFile();

private:

	UFUNCTION()
	void PlayMedia(UFileMediaSource* mediaSource);

	UFUNCTION()
	void StopMedia();

	class ARRightButton* RightButton;

	UFUNCTION()
	void EnableClickButton();

	FTimerHandle DelayTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Convayer")
	UMaterial* ConveyerMaterial;  // The base material reference in the editor

	UPROPERTY(VisibleAnywhere, Category = "Convayer")
	UMaterialInstanceDynamic* DynamicConveyerMaterialInstance;  // The dynamic material instance

	UFUNCTION()
	void HoveredCharacterIndexChange(int newIndex);

	class AEOSPlayerState* MyPlayerState;

	class AClipboard* Clipboard;

	UFUNCTION(Server, Reliable)
	void ServerRequestButtonClick(AEOSPlayerState* requestingPlayerState);

	UFUNCTION(Server, Reliable)
	void ServerRequestNextClick(AEOSPlayerState* requestingPlayerState);

	UFUNCTION(Server, Reliable)
	void ServerRequestSecretClick(AEOSPlayerState* requestingPlayerState);

	void GetCameraView();

	class ACineCameraActor* CineCamera;

	//class ALevelSequenceActor* MainMenuSequence;

	void PostPossessionSetup(APawn* NewPawn);

	void CreateCharacterSelectUI();

	void CreateLoadingScreenUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCharacterSelect> CharacterSelectUIClass;

	UPROPERTY()
	class UCharacterSelect* CharacterSelectUI;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULoadingScreenUI> LoadingScreenUIClass;

	UPROPERTY()
	class ULoadingScreenUI* LoadingScreenUI;

	UFUNCTION()
	void OnSequenceEnd();

	UPROPERTY()
	class AEOSGameState* GameState;

private:

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector ShownCage;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector OffScreen;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector Sideline;

	UPROPERTY(VisibleAnywhere, Category = "CagedCharacter")
	TArray<class ACagedCharacter*> CagedCharacters;

	void GetCagedCharacters();

	/*		Audio		*/
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ConveyorAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ConveyorStopAudio;

	void PlayConveyorStop();
};
