// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RPhysicalButtonBase.h"
#include "RRightButton.generated.h"

/**
 * 
 */
UCLASS()
class ARRightButton : public ARPhysicalButtonBase
{
	GENERATED_BODY()

private:
	FTimerHandle DelayTimerHandle;

	bool bPressDelay;

	void DelayTimer();

	UPROPERTY(EditDefaultsOnly, Category = "Convayer")
	UMaterial* ConveyerMaterial;  // The base material reference in the editor
	
	UPROPERTY(VisibleAnywhere, Category = "Convayer")
	UMaterialInstanceDynamic* DynamicConveyerMaterialInstance;  // The dynamic material instance

	class ARCharacterSelectMode* GameMode;

	class AClipboard* Clipboard;
	
public:
	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	virtual void BeginPlay() override;
};
