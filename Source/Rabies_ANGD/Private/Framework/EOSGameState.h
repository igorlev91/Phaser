// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionNameReplicated, const FName&, name);
/**
 * 
 */
UCLASS()
class AEOSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	FOnSessionNameReplicated OnSessionNameReplicated;

	FORCEINLINE FName GetSessionName() const { return SessionName; };

	void SetSessionName(const FName& updatedSessionName);
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	UFUNCTION()
	void OnRep_SessionName();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
