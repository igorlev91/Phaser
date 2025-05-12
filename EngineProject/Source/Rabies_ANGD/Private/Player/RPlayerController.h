// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	//this function calls only on the server
	virtual void OnPossess(APawn* NewPawn) override;

	//this function calls on both the listening server, and the client.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

	UFUNCTION()
	void ChangeCrosshairState(bool state);

	UFUNCTION()
	void ChangeTakeOffState(bool state, float charge);

	UFUNCTION()
	void ChangeRevivalState(bool state, float charge);

	UFUNCTION()
	void AddNewItemToUI(class URItemDataAsset* newItemAsset);

	UFUNCTION()
	bool HasGameplayUI();

	UFUNCTION()
	bool CashMyLuck();


	UFUNCTION()
	void GameOver();

	UFUNCTION()
	void OpenToyBox(class AItemPickup* itemToChange, FString itemSelection);

	UFUNCTION()
	void CloseToyBox(AItemPickup* choosingItem, URItemDataAsset* chosenItem);

	UFUNCTION(Server, Reliable)
	void ChangeItem(AItemPickup* choosingItem, URItemDataAsset* chosenItem);

	UFUNCTION()
	bool ManualDeathStatusUpdate(bool state);


	UFUNCTION(Server, Reliable)
	void Server_RequestRevive(AEOSPlayerState* TargetPlayerState);

	UFUNCTION()
	void AddBossEnemy(int level, class AREnemyBase* bossEnemy);

	UFUNCTION()
	void TogglePauseMenu(bool state);

	UFUNCTION()
	void ToggleUIState(bool state);

	UFUNCTION()
	void SensChange(float newSens);

private:

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> EndCutscene;

	void TryPossessExistingPawn();

	void CreateLoadingScreenUI();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULoadingScreenUI> LoadingScreenUIClass;

	UPROPERTY()
	class ULoadingScreenUI* LoadingScreenUI;

	void FixInput();

	FTimerHandle FixHandle;

	UFUNCTION(Server, Reliable)
	void FullyDead();

	void PostPossessionSetup(APawn* NewPawn);

	void CreateGameplayUI();

	void CreatePauseUI();

	UFUNCTION(Client, Reliable)
	void ImplimentBossEnemy(int level, class AREnemyBase* bossEnemy);

	UPROPERTY()
	class ARPlayerBase* PlayerBase;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameplayUI> GameplayUIClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	UGameplayUI* GameplayUI;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPauseUI> PauseUIClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	UPauseUI* PauseUI;
};
