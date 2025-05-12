// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestSpawnLocation.generated.h"

UCLASS()
class AChestSpawnLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChestSpawnLocation();

	UPROPERTY(EditDefaultsOnly, Category = "Details")
	bool bRareChest;

	UPROPERTY(Replicated)
	class AItemChest* OwningChest;

private:
	UPROPERTY(VisibleAnywhere, Category = "Details")
	class UStaticMeshComponent* DefaultApparence;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
