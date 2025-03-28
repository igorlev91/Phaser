// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSActionGameState.generated.h"

class AREnemyBase;
class AItemChest;
class AItemPickup;
class URItemDataAsset;

/**
 * 
 */
UCLASS()
class AEOSActionGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void SelectEnemy(AREnemyBase* selectedEnemy, bool isDeadlock, bool bIsDeadlockComponent);

	UFUNCTION()
	void SelectChest(AItemChest* openedChest);

	UFUNCTION()
	void SelectItem(AItemPickup* selectedItem, class ARPlayerBase* targetingPlayer);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AwardEnemyKill(TSubclassOf<class UGameplayEffect> rewardEffect);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartBossFight(int enemyID);

private:

	UPROPERTY()
	FVector deadlockPos;

	FTimerHandle WaveHandle;

	UPROPERTY()
	class AEscapeToWin* EscapeToWin;

	UPROPERTY()
	float WaveTime;

	UPROPERTY()
	int WaveLevel;

	UFUNCTION()
	void WaveSpawn(float timeToNextWave);

	UFUNCTION()
	void SpawnEnemyWave(int amountOfEnemies);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int AmountOfChests;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyInitalSpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyMax;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnChest(FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnEnemy(int EnemyIDToSpawn, FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenedChest(int chestID);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RemoveEnemy(int enemyID);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PickedUpItem(int itemID, class ARPlayerBase* targetingPlayer);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<AItemChest> ItemChestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<AItemPickup> ItemPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TArray<TSubclassOf<class AREnemyBase>> EnemyLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemSelection;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	URItemDataAsset* KeyCard;


	UPROPERTY(Replicated)
	TArray<AItemChest*> AllChests;

	UPROPERTY()
	int MaxChests;

	UPROPERTY()
	bool bGottenKeyCard = false;

	UFUNCTION()
	bool GetKeyCard();

	UPROPERTY(Replicated)
	TArray<AItemPickup*> AllItems;

	UPROPERTY(Replicated)
	TArray<class AREnemyBase*> AllEnemies;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
