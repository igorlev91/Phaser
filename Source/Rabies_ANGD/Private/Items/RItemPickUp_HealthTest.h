// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/RItemPickUpBase.h"
#include "RItemPickUp_HealthTest.generated.h"

/**
 * 
 */
UCLASS()
class ARItemPickUp_HealthTest : public ARItemPickUpBase
{
	GENERATED_BODY()

private:
	virtual void GiveUpgrade() override;
	
};
