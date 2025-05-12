// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "REOSGameSession.generated.h"

/**
 * 
 */
UCLASS()
class AREOSGameSession : public AGameSession
{
	GENERATED_BODY()
	virtual bool ProcessAutoLogin() override;
};
