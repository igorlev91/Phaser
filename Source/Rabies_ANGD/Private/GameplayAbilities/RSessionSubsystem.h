// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool);

/**
 * //https://cedric-neukirchen.net/docs/session-management/sessions-in-cpp/

 +
 */

UCLASS()
class URSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	URSessionSubsystem();

	void StartSession();
	void UpdateSession();
	void CreateSession(int32 NumPublicConnections, bool IsLANMatch);

	FOnSessionCreated OnCreateSessionEvent;

protected:
	void OnCreateSessionCompleted(FName SessionName, bool Successful);

private:
	FOnCreateSessionCompleteDelegate CreateSessionDelegate;
	FDelegateHandle CreateSessionDelegateHandle;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
};
