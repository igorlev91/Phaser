// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemChest.generated.h"

UCLASS()
class AItemChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemChest();

private:
	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* ChestBottomMesh;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* ChestTopMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);
};
