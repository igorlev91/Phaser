// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "RBoltHead_Head.generated.h"

/**
 * 
 */
UCLASS()
class URBoltHead_Head : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
private:
	URBoltHead_Head();

public:
	void WeeWoo(class ARPlayerBase* damagedPlayer, float movementSpeed, float ReviveSpeed);


private:
	
	FTimerHandle FinishedHandle;


};
