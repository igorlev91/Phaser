// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RPhysicalButtonBase.h"
#include "RLeftButton.generated.h"

/**
 * 
 */
UCLASS()
class ARLeftButton : public ARPhysicalButtonBase
{
	GENERATED_BODY()
	
public:
	ARLeftButton();

	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

	void ConfirmCharacter();


private:
	bool bSelected;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInstance* ErrorMaterialInstance;  // The dynamic material instance

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	/*		Audio		*/
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* SelectAudio;

	bool bPlaySound = true;
};
