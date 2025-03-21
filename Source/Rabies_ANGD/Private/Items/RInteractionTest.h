// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RInteractionTest.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class ARItemPickUpBase;
class USceneComponent;

UCLASS()
class ARInteractionTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARInteractionTest();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InteractMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* SphereCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/////////////////////////////////
	/*        Spawn Item	       */
	////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class ARItemPickUpBase> ItemSpawned;


public:
	UFUNCTION()
	void SpawnItem();

};