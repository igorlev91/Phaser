// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CagedCharacter.generated.h"

UCLASS()
class ACagedCharacter : public AActor
{
	GENERATED_BODY()

private:

	FTimerHandle MovementTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Cage Detail")
	class UStaticMeshComponent* CagedMesh;

	UPROPERTY(VisibleAnywhere, Category = "Cage Detail")
	class USkeletalMeshComponent* CagedCharacter;
	
public:	
	// Sets default values for this actor's properties
	ACagedCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Cage Detail")
	int CharacterIndex;

	UFUNCTION()
	void ClearTimer();

	UFUNCTION()
	void TickPosition(FVector Goal, FVector Sideline, bool goToSideline);

	UPROPERTY(EditDefaultsOnly, Category = "Cage Detail")
	class URCharacterDefination* Character;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
