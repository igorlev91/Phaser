// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RPhysicalButtonBase.h"
#include "RSecretButton.generated.h"

/**
 * 
 */
UCLASS()
class ARSecretButton : public ARPhysicalButtonBase
{
	GENERATED_BODY()

public:
	ARSecretButton();

	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);


protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	/*		Audio		*/
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* SelectAudio;

	bool bPlaySound = false;

	bool bSelected;
};
