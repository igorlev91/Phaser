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

public:
	ARRightButton();

private:

	bool bPressDelay;

	class AEOSGameState* GameState;

	class AClipboard* Clipboard;

	void ReEnableNextButton();
	
public:
	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

	UFUNCTION()
	void EnableClick();

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	virtual void BeginPlay() override;

	/*		Audio		*/
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* SelectAudio;

	bool bPlaySound = true;

	void SetPlaySoundToTrue();
};
