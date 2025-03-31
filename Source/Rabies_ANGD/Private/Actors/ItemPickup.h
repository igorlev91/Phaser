// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

class URItemDataAsset;

UCLASS()
class AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Item Detail")
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item Detail")
	class UStaticMeshComponent* SphereStyle;

	UPROPERTY(VisibleAnywhere, Category = "ChestDetail")
	class USphereComponent* SphereCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UStaticMesh* NullMesh;

	class ARPlayerBase* Player;

public:
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_PickupItem();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayerPickupRequest(class ARPlayerBase* player);

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateItemPickedup();

	UPROPERTY(VisibleAnywhere, Category = "ItemEffect")
	URItemDataAsset* ItemAsset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Unreliable)
	void SetupItem(URItemDataAsset* newItemAsset);
};
