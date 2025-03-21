// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPhysicalButtonBase.generated.h"

UCLASS()
class ARPhysicalButtonBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARPhysicalButtonBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

	UPROPERTY()
	class ARCharacterSelectController* CharacterSelectController;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MyMeshComponent;

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
};
