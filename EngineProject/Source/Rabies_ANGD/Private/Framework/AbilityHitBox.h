// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityHitBox.generated.h"

UCLASS()
class AAbilityHitBox : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AAbilityHitBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void SetIndex(int newIndex);

	UPROPERTY()
	class AClipboard* OwnerClipboard;

	UPROPERTY(EditAnywhere, Category = "AAAAA Index")
	int index;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* MyMeshComponent;

protected:

	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

};
