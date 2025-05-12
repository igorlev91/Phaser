// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivationTest.generated.h"

UCLASS()
class AActivationTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActivationTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class AEscapeToWin* EscapeGame;

private:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* ActivationWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class USphereComponent* SphereCollider;

	UPROPERTY(EditAnywhere, Category = "EndGame")
	bool bActivateCard;

	UPROPERTY(EditAnywhere, Category = "EndGame")
	bool bDefeatBoss;

	class ARPlayerBase* player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Escape")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SetUp();

	UFUNCTION()
	void SetActivationCard();

	UFUNCTION()
	void SetDefeatBoss();

};
