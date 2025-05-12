// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingActor.generated.h"

class URItemDataAsset;
UCLASS()
class APingActor : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* PingUIWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPingUI> PingUIClass;

	UPROPERTY()
	class UPingUI* PingUI;

	FTimerHandle DestroyTimer;
	
public:	
	// Sets default values for this actor's properties
	APingActor();

public:
	void SetIcons(UTexture* imageIcon, FText text);

	void UpdatePingUI(UTexture* imageIcon, FText text);

	UFUNCTION()
	void TimedDestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private: 

	UPROPERTY(ReplicatedUsing = OnRep_Icon)
	UTexture* IconTexture;

	UPROPERTY(ReplicatedUsing = OnRep_Text)
	FText IconText;

	UFUNCTION()
	void OnRep_Icon();

	UFUNCTION()
	void OnRep_Text();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

};
