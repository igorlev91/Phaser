// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "EOSGameState.generated.h"

/**
 * 
 */
UCLASS()
class AEOSGameState : public AGameState
{
	GENERATED_BODY()

public:
	FORCEINLINE FName GetSessionName() const { return SessionName; };

	void SetSessionName(const FName& updatedSessionName);
	
private:
	
	UPROPERTY(Replicated)
	FName SessionName;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
