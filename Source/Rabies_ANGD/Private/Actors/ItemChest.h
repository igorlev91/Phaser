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

	UPROPERTY(VisibleAnywhere, Category = "ChestDetail")
	class USphereComponent* SphereCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "InteractionWidget")
	TSubclassOf<class UUserWidget> InteractionWidget;

	UUserWidget* WidgetInstance;
};
