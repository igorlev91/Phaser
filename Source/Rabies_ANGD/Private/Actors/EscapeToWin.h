// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapeToWin.generated.h"

UCLASS()
class AEscapeToWin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEscapeToWin();

	UPROPERTY(Replicated)
	int hasBeatenBoss = 0;

	bool bHasKeyCard = false;

	UPROPERTY(Replicated)
	bool bStartBoss = false;

protected:
	bool bHasWonGame = false;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class USphereComponent* SphereCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Escape")
	class UStaticMeshComponent* EndGameMesh;


private:
	UFUNCTION()
	void Interact();



	/*	Widget Compositions	 */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* EndGameWidgetComp;

	/*	Widgets	 */
	UPROPERTY(Replicated)
	class UEndGameWidget* EndGameUI;

	//Transient - Reference to the widget

	class ARPlayerBase* player;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(replicatedUsing = OnRep_CurrentText)
	FString currentUIText;

	UPROPERTY(replicatedUsing = OnRep_CurrentColor)
	FLinearColor currentUIColor;

	UFUNCTION()
	void OnRep_CurrentText();

	UFUNCTION()
	void OnRep_CurrentColor();

	UFUNCTION(NetMulticast, Unreliable)
	void ChangeText(const FString& newText, FLinearColor newColor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void UseKeycard();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void CheckKeyCard();

	UFUNCTION(BlueprintCallable, Category = "Escape")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Escape")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*	UI Setup  */
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void SetUpEndGame();

	UFUNCTION()
	void SetActivatingExit();

	UFUNCTION()
	void EndGame();

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateEscapeToWin();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

};
