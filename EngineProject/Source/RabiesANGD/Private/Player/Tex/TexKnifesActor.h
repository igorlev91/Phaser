// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TexKnifesActor.generated.h"

UCLASS()
class ATexKnifesActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATexKnifesActor();

private:
	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* knife1;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* knife2;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* knife3;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* knife4;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* knife5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetActiveState(bool state);

};
