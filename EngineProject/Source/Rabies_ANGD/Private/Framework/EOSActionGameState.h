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
	ESettingsType GetSettings() { return GameSettings; }

	UFUNCTION()
	void ChooseDifficulty(ESettingsType settingsType);

	UFUNCTION()
	void SelectEnemy(AREnemyBase* selectedEnemy, bool isDeadlock, bool bIsDeadlockComponent);

	UFUNCTION()
	void SelectChest(AItemChest* openedChest, int spawnCount);

	UFUNCTION()
	void SelectItem(AItemPickup* selectedItem, class ARPlayerBase* targetingPlayer);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void AwardEnemyKill(TSubclassOf<class UGameplayEffect> rewardEffect);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartBossFight(int enemyID);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void LeaveLevel();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnDotLaserMarks(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnVFXOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Server_RequestChangeItem(AItemPickup* ChoosingItem, URItemDataAsset* chosenItem);


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpawnVFX(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestSpawnVFX(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestSpawnVFXOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AdjustFireOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AdjustIceOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AdjustIceOnCharacter(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShootTexUltimate(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, FVector endPos);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RobotGiblets(FVector SpawnLocation, FVector Direction, int amount);


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnHealingPulse(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float healingRadius);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnRadio(UNiagaraSystem* SystemToSpawn, class ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float size);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnItemOnPlayer(URItemDataAsset* itemData, ARPlayerBase* targetingPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerFullyDies(ARPlayerBase* deadPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RequestPlayAudio(USoundBase* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Ping(FVector hitPoint, AActor* hitActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Ping(FVector hitPoint, AActor* hitActor);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnDeathItem(AREnemyBase* enemyDropping, float propability, int itemsToDrop);


private:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAudio();

	UFUNCTION(NetMulticast, Reliable)
	void CrossfadeMusic();

	UPROPERTY()
	UAudioComponent* MainAudioComponent;

	UPROPERTY()
	UAudioComponent* BossComponent;


	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundAttenuation* VoiceAttenuationSettings;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* MainAudio;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* BossAudio;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* DeadlockSpawnAudio;

	UPROPERTY(Replicated)
	ESettingsType GameSettings;

	UFUNCTION()
	void STARTGAME();

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* ChestIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* GateIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Ping")
	TSubclassOf<class APingActor> PingActorClass;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnDotLaserMarks(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HealingPulse;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* RobotGiblets;

	UFUNCTION()
	TArray<URItemDataAsset*> GetItemPoolOfRarity(TArray<URItemDataAsset*> itemPool, EItemRarity itemRarity);

	UPROPERTY(EditDefaultsOnly, Category = "Deadlock")
	FVector deadlockPos;

	FTimerHandle WaveHandle;
	FTimerHandle SpawnHandle;
	FTimerHandle ChestSpawnHandle;

	void LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> SelectLevel;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> CinematicEnding;


	UPROPERTY()
	class AEscapeToWin* EscapeToWin;

	UPROPERTY()
	float WaveTime;

	UPROPERTY()
	int WaveLevel;

	UFUNCTION()
	void WaveSpawn(float timeToNextWave);

	UFUNCTION()
	void SpawnEnemyWave(int powerLevel);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	bool devDiff;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int IncreasedPower;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int AmountOfChests;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int ChestSpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	bool bToyBox;

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	bool bEnemyItemDrops;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyInitalSpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemySpawnRateChange;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int enemyMax;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	int WaveSpawnCap;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnChest(float randomItemCost);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void PeriodicSpawnChest();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnEnemy(int EnemyIDToSpawn, FVector SpawnLocation, bool fromEnemySelection);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenedChest(int chestID, int spawnCount);

	UFUNCTION()
	void SpawnItem(FVector SpawnLocation, float cost, int spawnCount);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void DeleteChest(int chestID, float timeToDisappear);

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

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TArray<TSubclassOf<class AREnemyBase>> EnemySelection;

	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TArray<int> EnemyIndexPowerLevels;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> SinglePlayerItemLibrary;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ItemSelection;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ToyBoxCommon;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ToyBoxUncommon;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<URItemDataAsset*> ToyBoxRare;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	URItemDataAsset* KeyCard;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Items")
	bool bInstantKeyCard;


	UPROPERTY(Replicated)
	TArray<AItemChest*> AllChests;

	UPROPERTY(Replicated)
	TArray<class AChestSpawnLocation*> AllChestsSpawnLocations;

	UPROPERTY()
	int MaxChests;

	UPROPERTY(Replicated)
	int OpenedChests;

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
