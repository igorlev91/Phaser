// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSActionGameState.generated.h"

/**
 * 
 */
UCLASS()
class AEOSActionGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

private:

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	int AmountOfChests;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnChest(FVector SpawnLocation);

	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TSubclassOf<class AItemChest> ItemChestClass;

	UPROPERTY(Replicated)
	TArray<class AItemChest*> AllChests;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
