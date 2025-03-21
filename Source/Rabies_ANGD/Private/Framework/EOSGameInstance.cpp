// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"


#include "Kismet/GameplayStatics.h"

void UEOSGameInstance::Login()
{
	if (identityPtr)
	{
		FOnlineAccountCredentials onlineAccountCredentials;
		onlineAccountCredentials.Type = "accountportal";
		onlineAccountCredentials.Id = "";
		onlineAccountCredentials.Token = "";
		identityPtr->Login(0, onlineAccountCredentials);
	}
}

void UEOSGameInstance::CreateSession(const FName& SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to create"));
	if (sessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bUsesPresence= true;
		SessionSettings.NumPublicConnections = 10;

		SessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		sessionPtr->CreateSession(0, SessionName, SessionSettings);
	}
}

void UEOSGameInstance::FindSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Session"));

	sessionSearch = TSharedPtr<FOnlineSessionSearch>(new FOnlineSessionSearch);//MakeShareable(new FOnlineSessionSearch);

	sessionSearch->bIsLanQuery = false;
	sessionSearch->MaxSearchResults = 10;
	sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	sessionPtr->FindSessions(0, sessionSearch.ToSharedRef());
}

void UEOSGameInstance::JoinSessionWithSearchResultIndex(int SearchResultIndex)
{
	if (SearchResultIndex < 0 || SearchResultIndex >= sessionSearch->SearchResults.Num())
	{
		return;
	}

	const FOnlineSessionSearchResult& SearchResult = sessionSearch->SearchResults[SearchResultIndex];

	FString SessionName;
	SearchResult.Session.SessionSettings.Get(GetSessionNameKey(), SessionName);
	sessionPtr->JoinSession(0, FName{ SessionName }, SearchResult);
}

FString UEOSGameInstance::GetSessionName(const FOnlineSessionSearchResult& SearchResult) const
{
	FString searchResultValue = "Name None";
	SearchResult.Session.SessionSettings.Get(GetSessionNameKey(), searchResultValue);
	return searchResultValue;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	onlineSubsystem =  IOnlineSubsystem::Get();
	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::LoginCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();
	sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::CreateSessionCompleted);

	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindSessionsCompleted);

	sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::JoinSessionCompleted);
}

void UEOSGameInstance::LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logged in"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Login"));
	}


}

void UEOSGameInstance::CreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Session completed"));

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logged into session"));

		if (!GameLevel.IsValid())
		{
			GameLevel.LoadSynchronous();
		}

		if (GameLevel.IsValid())
		{
			const FName levelName = FName(*FPackageName::ObjectPathToPackageName(GameLevel.ToString()));
			GetWorld()->ServerTravel(levelName.ToString() + "?listen");
		}
	}
}

void UEOSGameInstance::FindSessionsCompleted(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Session"));

	if (bWasSuccessful)
	{
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults)
		{
			FString LobbyName = GetSessionName(lobbyFound);
			UE_LOG(LogTemp, Warning, TEXT("Found sess: %s"), *lobbyFound.GetSessionIdStr());
			lobbyFound.GetSessionIdStr();
		}

		SearchCompleted.Broadcast(sessionSearch->SearchResults);

		JoinSessionWithSearchResultIndex(0);
	}
}

void UEOSGameInstance::JoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session: %s completed"), *sessionName.ToString())
		FString TravelURL;
		sessionPtr->GetResolvedConnectString(sessionName, TravelURL);
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
}

