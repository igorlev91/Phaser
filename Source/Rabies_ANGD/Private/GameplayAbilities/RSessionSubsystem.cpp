// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/RSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "OnlineKeyValuePair.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

URSessionSubsystem::URSessionSubsystem() : 
	CreateSessionDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)) { }

void URSessionSubsystem::StartSession()
{
	const IOnlineSessionPtr currentSessionInterface = Online::GetSessionInterface(GetWorld());
	if (!currentSessionInterface.IsValid())
	{
		OnCreateSessionEvent.Broadcast(false);
		return; // the session does not exist
	}


}

void URSessionSubsystem::UpdateSession()
{
	const IOnlineSessionPtr currentSessionInterface = Online::GetSessionInterface(GetWorld());
	if (!currentSessionInterface.IsValid())
	{
		OnCreateSessionEvent.Broadcast(false);
		return; // the session does not exist
	}

	TSharedPtr<FOnlineSessionSettings> updatedSessionSettings = MakeShareable(new FOnlineSessionSettings(*LastSessionSettings));
	updatedSessionSettings->Set("Facility_Courtyard", FString("Facility_Courtyard"), EOnlineDataAdvertisementType::ViaOnlineService);

	CreateSessionDelegateHandle = currentSessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(CreateSessionDelegate);

	if (!currentSessionInterface->UpdateSession(NAME_GameSession, *updatedSessionSettings))
	{
		currentSessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);

		OnCreateSessionEvent.Broadcast(false);
	}
	else
	{
		LastSessionSettings = updatedSessionSettings;
	}
}

void URSessionSubsystem::CreateSession(int32 NumPublicConnections, bool IsLANMatch)
{
	const IOnlineSessionPtr currentSessionInterface = Online::GetSessionInterface(GetWorld());
	if (!currentSessionInterface.IsValid())
	{
		OnCreateSessionEvent.Broadcast(false);
		return; // the session does not exist
	}

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = IsLANMatch;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set("Facility_Courtyard", FString("Facility_Courtyard"), EOnlineDataAdvertisementType::ViaOnlineService); //this first argument might make it crash, not sure yet

	CreateSessionDelegateHandle = currentSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!currentSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		currentSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
		OnCreateSessionEvent.Broadcast(false);
	}

}

void URSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool Successful)
{
	const IOnlineSessionPtr currentSessionInterface = Online::GetSessionInterface(GetWorld());

	if (currentSessionInterface)
	{
		currentSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegateHandle);
	}

	OnCreateSessionEvent.Broadcast(true);
}