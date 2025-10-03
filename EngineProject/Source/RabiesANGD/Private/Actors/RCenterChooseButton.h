// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/RPhysicalButtonBase.h"
#include "RCenterChooseButton.generated.h"

/**
 * 
 */
UCLASS()
class ARCenterChooseButton : public ARPhysicalButtonBase
{
	GENERATED_BODY()
	
public:

	void OnActorClicked(AActor* TouchedActor, FKey ButtonPressed);

protected:
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
};
