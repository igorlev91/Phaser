// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RabiesGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ARabiesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
private:	
	virtual void PostLogin(APlayerController* PlayerController) override;
	virtual void Logout(AController* PlayerController) override;
};
