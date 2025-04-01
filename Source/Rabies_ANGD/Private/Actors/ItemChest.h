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

	bool bWithinInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UMaterialInstance* NegativeOutline;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* ChestBottomMesh;

	UPROPERTY(VisibleAnywhere, Category = "Chest Detail")
	class UStaticMeshComponent* ChestTopMesh;

	UPROPERTY(VisibleAnywhere, Category = "ChestDetail")
	class USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* InteractWidgetComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* InvalidScrapAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ChestOpenAudio;

	UPROPERTY()
	class UChestInteractUI* InteractWidget;

	class ARPlayerBase* player;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> ScrapPriceEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, Transient, EditDefaultsOnly, Category = "Abilities")
	float ScrapPrice;

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SetUpUI(bool SetInteraction);

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateChestOpened();

	UFUNCTION()
	void SetPairedPing(class APingActor* myPing);

	UFUNCTION()
	bool HasPing();

private:
	UPROPERTY(Replicated)
	class APingActor* MyPing;


	UPROPERTY(Replicated)
	bool bWasOpened;

	UFUNCTION()
	void Interact();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

public:
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_OpenChest(bool bFeelinLucky);

};
