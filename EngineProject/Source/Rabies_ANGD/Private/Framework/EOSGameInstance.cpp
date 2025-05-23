// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "TimerManager.h"

#include "Engine/World.h"

#include "Player/RMainMenuController.h"

#include "Kismet/GameplayStatics.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

void UEOSGameInstance::Login()
{
	//MenuController->ChangeMainMenuState(false);
	//MenuController->ChangeOnlineMenuState(true);
	//identityPtr->AutoLogin(0);
	
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	if (identityPtr)
	{
		int32 loginStatus = identityPtr->GetLoginStatus(0);

		if (loginStatus == 2) //player is already logged in
		{
			if (MenuController)
			{
				MenuController->OpenLoadingScreen(1);
				GetWorld()->GetTimerManager().SetTimer(LoginDisplayDelayHandle, this, &UEOSGameInstance::LoginDisplayDelay, 1.0f);
				UE_LOG(LogTemp, Warning, TEXT("Already Logged in"));
			}
		}
		else
		{
			FOnlineAccountCredentials onlineAccountCredentials;
			onlineAccountCredentials.Type = "accountportal";
			onlineAccountCredentials.Id = "";
			onlineAccountCredentials.Token = "";
			identityPtr->Login(0, onlineAccountCredentials);
		}

		UE_LOG(LogTemp, Warning, TEXT("Login Status: %d"), loginStatus);
	}
}

void UEOSGameInstance::LoginDisplayDelay()
{
	MenuController->ChangeMainMenuState(false);
	MenuController->ChangeOnlineMenuState(true);
}


void UEOSGameInstance::LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	if (bWasSuccessful)
	{
		MenuController->OpenLoadingScreen(1);
		GetWorld()->GetTimerManager().SetTimer(LoginDisplayDelayHandle, this, &UEOSGameInstance::LoginDisplayDelay, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Logged in"));
	}
	else
	{
		MenuController->ChangeMainMenuState(true);
		MenuController->ChangeOnlineMenuState(false);
		UE_LOG(LogTemp, Warning, TEXT("Failed to Login"));
	}
}

void UEOSGameInstance::CreateSession(const FName& SessionName)
{
	/*if (sessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsDedicated = true;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bUseLobbiesIfAvailable = false;

		//SessionSettings.bAllowInvites = true;
		//SessionSettings.bAllowJoinViaPresence = true;
		//SessionSettings.bUsesPresence = false;

		SessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		CurrentLobbyName = SessionName;

		sessionPtr->CreateSession(0, SessionName, SessionSettings);
	}*/

	//if(SessionName)
	// RIGHT HERE, CHECK TO SEE IF YOU ALREADY HAVE THE SESSION NAME HERE

	FGuid NewSesionUniqueId = FGuid::NewGuid();
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	FString CoordinatorURLStr = GetCoordinatorURLStr();
	FString RequestURL = FString::Printf(TEXT("http://%s/StartServer"), *(CoordinatorURLStr));
	Request->SetURL(RequestURL);
	Request->SetVerb("POST");

	Request->SetHeader(GetSessionGUIDNameKey().ToString(), NewSesionUniqueId.ToString());
	Request->SetHeader(GetSessionNameKey().ToString(), SessionName.ToString());

	Request->OnProcessRequestComplete().BindUObject(this, &UEOSGameInstance::SesssionCreationRequestCompleted, NewSesionUniqueId);
	Request->ProcessRequest();
}

void UEOSGameInstance::FindSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Finding Session"));

	sessionSearch = TSharedPtr<FOnlineSessionSearch>(new FOnlineSessionSearch);//MakeShareable(new FOnlineSessionSearch);

	sessionSearch->bIsLanQuery = false;
	sessionSearch->MaxSearchResults = 15;
	//sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, false, EOnlineComparisonOp::Equals);

	sessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindSessionsCompleted);
	if (!sessionPtr->FindSessions(0, sessionSearch.ToSharedRef()))
	{
		sessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		UE_LOG(LogTemp, Warning, TEXT("Find Session Failed"))
	}
}

void UEOSGameInstance::JoinSessionWithSearchResultIndex(int SearchResultIndex)
{
	//TODO: Remove this? is it even used
	if (SearchResultIndex < 0 || SearchResultIndex >= sessionSearch->SearchResults.Num())
	{
		return;
	}

	const FOnlineSessionSearchResult& SearchResult = sessionSearch->SearchResults[SearchResultIndex];

	FString SessionName;
	SearchResult.Session.SessionSettings.Get(GetSessionNameKey(), SessionName);

}

void UEOSGameInstance::JoinLobbyBySearchIndex(int index)
{
	if (index < 0 || index >= sessionSearch->SearchResults.Num())
	{
		return;
	}

	const auto& SearchResult = sessionSearch->SearchResults[index];
	FString SessionName = GetSessionName(SearchResult);

	FString SessionPortStr = "";
	SearchResult.Session.SessionSettings.Get(FName("PORT"), SessionPortStr);

	int Port = FCString::Atoi(*SessionPortStr);

	sessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::JoinSessionCompleted, Port);

	UE_LOG(LogTemp, Warning, TEXT("Attempting to joing session"))
	if (!sessionPtr->JoinSession(0, FName{ SessionName }, SearchResult))
	{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to joing session failed right away"))

		sessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

FString UEOSGameInstance::GetSessionName(const FOnlineSessionSearchResult& SearchResult) const
{
	FString searchResultValue = "Name None";
	SearchResult.Session.SessionSettings.Get(GetSessionNameKey(), searchResultValue);
	return searchResultValue;
}

void UEOSGameInstance::SetMenuController(ARMainMenuController* menuController)
{
	MenuController = menuController;
}

void UEOSGameInstance::Init()
{
	Super::Init();	
	
	onlineSubsystem =  IOnlineSubsystem::Get();
	identityPtr = onlineSubsystem->GetIdentityInterface();
	identityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::LoginCompleted);

	sessionPtr = onlineSubsystem->GetSessionInterface();

	//sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::CreateSessionCompleted);

	//sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindSessionsCompleted);

	//sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::JoinSessionCompleted);

	UE_LOG(LogTemp, Warning, TEXT("net mode is: %d"), GetWorld()->GetNetMode())
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		CreateSessionBasedOnCLIArgs(); //THIS CRASHES IF IT'S NOT IN THE MAIN MENU
		return;
	}

	/*FString launchMode = "";
	FParse::Value(FCommandLine::Get(), TEXT("-LaunchMode="), launchMode);
	UE_LOG(LogTemp, Warning, TEXT("the launch mode string is: %s"), *(launchMode))
	if (launchMode == "TestLocalDedicatedServer")
	{
		UE_LOG(LogTemp, Warning, TEXT("loading select level"));
		CreateTestEOSSession();
		return;
	}*/

	if (AttemptAutoLogin())
	{
		UE_LOG(LogTemp, Warning, TEXT("successful autologin"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to login"));
	}
}

bool UEOSGameInstance::AttemptAutoLogin()
{
	if (identityPtr)
	{
		FString authType = "";
		FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), authType);
		if (!authType.IsEmpty())
		{
			return identityPtr->AutoLogin(0);
		}
	}
	
	return false;
}

void UEOSGameInstance::CreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Session completed"));

	if (bWasSuccessful)
	{
		SessionJoined.Broadcast();
		LoadMapAndListen(SelectLevel);
		sessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		if (NewSessionWiatClientTerminateHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(NewSessionWiatClientTerminateHandle);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("################ Start Session Close Countdown if no play joins"))
		GetWorld()->GetTimerManager().SetTimer(NewSessionWiatClientTerminateHandle, this, &UEOSGameInstance::TerminateSessionServer, 60.0f);
	}
	else
	{
		if (GEngine)
		{
			GEngine->Exec(GetWorld(), TEXT("QUIT"));
		}
	}
}

void UEOSGameInstance::DelayedLoad(int Port)
{	
	FString TravelURL;
	sessionPtr->GetResolvedConnectString(newSessionName, TravelURL);

	FURL Url{ nullptr, *TravelURL, ETravelType::TRAVEL_Absolute };
	Url.Port = Port;

	if (IsLocalTesting())
	{
		Url.Host = "127.0.0.1";
	}

	GetFirstLocalPlayerController(GetWorld())->ClientTravel(Url.ToString(), TRAVEL_Absolute);
}

void UEOSGameInstance::FindSessionsCompleted(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Session completed"));

	if (bWasSuccessful)
	{
		for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults)
		{
			FString LobbyName = GetSessionName(lobbyFound);
			UE_LOG(LogTemp, Warning, TEXT("Found sess: %s"), *lobbyFound.GetSessionIdStr());
			lobbyFound.GetSessionIdStr();
		}

		SearchCompleted.Broadcast(sessionSearch->SearchResults);

		//JoinSessionWithSearchResultIndex(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Finding Session completed but not successful"));
	}
}

void UEOSGameInstance::JoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type Result, int Port)
{
	UE_LOG(LogTemp, Warning, TEXT("Join session completed, verifying state: %d"), Result)
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session: %s completed"), *sessionName.ToString());
		SessionJoined.Broadcast();

		newSessionName = sessionName;
		GetWorld()->GetTimerManager().SetTimer(DelayedLoadTimer, FTimerDelegate::CreateUObject(this, &UEOSGameInstance::DelayedLoad, Port), 1.0f, false);
	}
}

void UEOSGameInstance::LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad)
{
	if (!levelToLoad.IsValid())
	{
		levelToLoad.LoadSynchronous();
	}

	if (levelToLoad.IsValid())
	{
		if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			
		}
		else
		{
			MenuController->CreateLoadingScreenUI();
		}

		const FName levelName = FName(*FPackageName::ObjectPathToPackageName(levelToLoad.ToString()));
		GetWorld()->ServerTravel(levelName.ToString() + "?listen");
	}
}

void UEOSGameInstance::CreateTestEOSSession()
{
	CreateSession("TestSession");
}

void UEOSGameInstance::StartFindCreatedSession(FGuid SessionUniqueId, int Port)
{
	StopFindingCreatedSession();
	MenuController->OpenLoadingScreen(2);
	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeoutHandle, this, &UEOSGameInstance::FindCreatedSessionTimeoutReached, FindCreatedSessionTimeroutSeconds);
	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimerHandle, FTimerDelegate::CreateUObject(this, &UEOSGameInstance::TryFindCreatedSession, SessionUniqueId, Port), FindCreatedSessionInterval, true);
}

void UEOSGameInstance::StopFindingCreatedSession()
{
	if (sessionPtr)
	{
		sessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}

	MenuController->ChangeMainMenuState(false);
	MenuController->ChangeOnlineMenuState(true);

	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
}

void UEOSGameInstance::TryFindCreatedSession(FGuid SessionUniqueId, int Port)
{
	UE_LOG(LogTemp, Warning, TEXT("Start Finding Session"));

	sessionSearch = TSharedPtr<FOnlineSessionSearch>(new FOnlineSessionSearch);//MakeShareable(new FOnlineSessionSearch);

	sessionSearch->bIsLanQuery = false;
	sessionSearch->MaxSearchResults = 15;
	//sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, false, EOnlineComparisonOp::Equals);

	sessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	sessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::FindCreatedSessionResultsReceived, SessionUniqueId, Port);
	if (!sessionPtr->FindSessions(0, sessionSearch.ToSharedRef()))
	{
		sessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		UE_LOG(LogTemp, Warning, TEXT("Find Session Failed"))
	}
}

void UEOSGameInstance::JoinCreatedSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type Result, int Port)
{
	UE_LOG(LogTemp, Warning, TEXT("Join session completed, verifying state: %d"), Result)
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session: %s completed"), *sessionName.ToString());
		SessionJoined.Broadcast();

		newSessionName = sessionName;
		
		FString TravelURL;
		sessionPtr->GetResolvedConnectString(newSessionName, TravelURL);

		FURL Url{nullptr, *TravelURL, ETravelType::TRAVEL_Absolute};
		Url.Port = Port;

		if (IsLocalTesting())
		{
			//Url.Host = "127.0.0.1";
			Url.Host = "10.40.14.25";
		}

		GetFirstLocalPlayerController(GetWorld())->ClientTravel(Url.ToString(), TRAVEL_Absolute);
	}
}

void UEOSGameInstance::FindCreatedSessionResultsReceived(bool bWasSuccessful, FGuid SessionUniqueId, int Port)
{
	if (!bWasSuccessful)
		return;

	int index = 0;
	for (const FOnlineSessionSearchResult& lobbyFound : sessionSearch->SearchResults)
	{
		FString LobbyName = GetSessionName(lobbyFound);
		UE_LOG(LogTemp, Warning, TEXT("Found session: %s"), *lobbyFound.GetSessionIdStr());
		lobbyFound.GetSessionIdStr();

		FString SessionUniqueIdStr = "";
		lobbyFound.Session.SessionSettings.Get(GetSessionGUIDNameKey(), SessionUniqueIdStr);
		if (SessionUniqueIdStr == SessionUniqueId.ToString())
		{
			FString SessionName = GetSessionName(lobbyFound);

			sessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
			sessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::JoinCreatedSessionCompleted, Port);
			UE_LOG(LogTemp, Warning, TEXT("Attempting to joing session"))
			if (!sessionPtr->JoinSession(0, FName{ SessionName }, lobbyFound))
			{
				UE_LOG(LogTemp, Warning, TEXT("Attempting to joing session failed right away"))

					sessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
			}

			StopFindingCreatedSession();

			if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
			{

			}
			else
			{
				MenuController->CreateLoadingScreenUI();
			}

			return;
		}
	}
}

void UEOSGameInstance::FindCreatedSessionTimeoutReached()
{
	StopFindingCreatedSession();
}

bool UEOSGameInstance::IsLocalTesting() const
{
	FString LocalTest = "";
	FParse::Value(FCommandLine::Get(), *FString::Printf(TEXT("%s="), *GetLocalTestArgName().ToString()), LocalTest);
	return LocalTest == "True";
}

void UEOSGameInstance::StartSession()
{
	if (sessionPtr)
	{
		sessionPtr->StartSession(FName(DedicateServerSessionName));
	}
}

void UEOSGameInstance::PlayerJoined(APlayerController* PlayerController)
{
	CurrentPlayerIds.Add(PlayerController->GetPlayerState<APlayerState>()->GetUniqueId());
	UE_LOG(LogTemp, Warning, TEXT("################ Player Joined, stop server termination"))
	GetWorld()->GetTimerManager().ClearTimer(NewSessionWiatClientTerminateHandle);
}

void UEOSGameInstance::PlayerLeft(AController* PlayerController)
{
	CurrentPlayerIds.Remove(PlayerController->GetPlayerState<APlayerState>()->GetUniqueId());
	if (CurrentPlayerIds.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("####### All Player left Terminate Session"))
		TerminateSessionServer();
	}
}

void UEOSGameInstance::TerminateSessionServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Terminate Session Sever Called, start shutting down!"))
	if (sessionPtr)
	{
		sessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::EndSessionCompleted);
		if (!sessionPtr->EndSession(FName{ DedicateServerSessionName }))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UEOSGameInstance::CreateSessionBasedOnCLIArgs()
{
	FString SessionGUID = "";
	FParse::Value(FCommandLine::Get(), *FString::Printf(TEXT("%s="), *GetSessionGUIDNameKey().ToString()), SessionGUID);
	
	FString SessionName = "";
	FParse::Value(FCommandLine::Get(), *FString::Printf(TEXT("%s="), *GetSessionNameKey().ToString()), SessionName);
	DedicateServerSessionName = SessionName;

	FString SessionPort = "";
	FString PortKeyName = "PORT";
	FParse::Value(FCommandLine::Get(), *FString::Printf(TEXT("%s="), *(PortKeyName)), SessionPort);

	UE_LOG(LogTemp, Warning, TEXT("Launching session with guid: %s, and session name: %s and port: %s"), *SessionGUID, *SessionName, *SessionPort)
	
	if (sessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsDedicated = true;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinInProgress = false;
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bUseLobbiesIfAvailable = false;
		//sessionPtr->StartSession();
		//SessionSettings.bAllowInvites = true;
		/*SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUsesPresence = false;*/

		SessionSettings.Set(GetSessionNameKey(), SessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(GetSessionGUIDNameKey(), SessionGUID, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(FName(PortKeyName), SessionPort, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		CurrentLobbyName = FName{ SessionName };

		sessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		sessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::CreateSessionCompleted);
		if (!sessionPtr->CreateSession(0, FName{ SessionName }, SessionSettings))
		{
			sessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		}
	}
}

FString UEOSGameInstance::GetCoordinatorURLStr() const
{
	FString CoordiantorURL = "";
	FParse::Value(FCommandLine::Get(), *FString::Printf(TEXT("%s="), *GetCoordinatorURLStrKey().ToString()), CoordiantorURL);
	UE_LOG(LogTemp, Warning, TEXT("Found Coordinator URL: %s"), *(CoordiantorURL));
	if (CoordiantorURL == "")
		return "3.148.242.156";

	return CoordiantorURL;
	//return "3.148.242.156";
}

void UEOSGameInstance::SesssionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bProcessdSuccessfully, FGuid SessionUniqueId)
{
	if (bProcessdSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully"));
		FString PortStr = Response->GetHeader("PORT");
		int Port = FCString::Atoi(*PortStr);
		StartFindCreatedSession(SessionUniqueId, Port);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session did not got created..."));
	}
}

void UEOSGameInstance::EndSessionCompleted(FName EndedSessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}