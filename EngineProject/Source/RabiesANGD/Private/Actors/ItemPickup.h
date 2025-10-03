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

	bool bRotate;

	UFUNCTION()
	void StartLookingForGround();

public:

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_PickupItem();

	UFUNCTION(Server, Unreliable)
	void PlayerPickupRequest(class ARPlayerBase* player);

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateItemPickedup();

	UPROPERTY(VisibleDefaultsOnly, Category = "ItemEffect")
	URItemDataAsset* ItemAsset;

	UFUNCTION(NetMulticast, Unreliable)
	void StartRotatingItem();

	FTimerHandle FallTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Unreliable)
	void SetupItem(URItemDataAsset* newItemAsset, FVector randomDirection);

	UFUNCTION()
	void SetPairedPing(class APingActor* myPing);

	UFUNCTION()
	bool HasPing();

private:
	UPROPERTY(Replicated)
	class APingActor* MyPing;

	UFUNCTION()
	void OnItemHit(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);


private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

};
