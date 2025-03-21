// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/RItemPickUpBase.h"
#include "RItemPickUp_Test.generated.h"

/**
 * 
 */
UCLASS()
class ARItemPickUp_Test : public ARItemPickUpBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade")
	float AddedSpeed;

private:
	virtual void GiveUpgrade() override;
	
};
