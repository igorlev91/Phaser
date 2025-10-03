// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChesterBallActor.generated.h"

UCLASS()
class AChesterBallActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChesterBallActor();

private:
	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* Ball;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetActiveState(bool state);

};
