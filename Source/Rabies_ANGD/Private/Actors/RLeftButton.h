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

	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);


private:


protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
};
