// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamagePopupActor.generated.h"

UCLASS()
class ADamagePopupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagePopupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetText(int damage, FVector animOffset);

	void UpdateDamageText(int damage);

private:

	FVector StartLocation;

	UPROPERTY(ReplicatedUsing = OnRep_AnimOffset)
	FVector AnimOffset;

	UFUNCTION()
	void TimedDestroy();

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* DamageWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDamagePopup> DamageUIClass;

	UPROPERTY()
	class UDamagePopup* DamageUI;

	FTimerHandle DestroyTimer;

	UPROPERTY(ReplicatedUsing = OnRep_Damage)
	int Damage;

	UFUNCTION()
	void OnRep_Damage();

	UFUNCTION()
	void OnRep_AnimOffset();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

};
