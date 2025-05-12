// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Interfaces/IHttpRequest.h"

#include "EOSGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionSearchCompleted, const TArray<class FOnlineSessionSearchResult>& /* Seach results */);
DECLARE_MULTICAST_DELEGATE(FOnSessionJoin);



/**
 * 
 */
UCLASS()
class UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	FOnSessionSearchCompleted SearchCompleted;
	FOnSessionJoin SessionJoined;

	void JoinLobbyBySearchIndex(int index);

	UFUNCTION(BlueprintCallable)
	void Login();

	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName& SessionName);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	const FName& GetSessionNameKey() const { return SessionNameKey; }

	void JoinSessionWithSearchResultIndex(int SearchResultIndex);

	FString GetSessionName(const FOnlineSessionSearchResult& SearchResult) const;
	FORCEINLINE FName GetCurrentSessionName() const { return CurrentLobbyName; }

	void SetMenuController(class ARMainMenuController* menuController);

protected:
	virtual void Init() override;

private:
	
	FTimerHandle DelayedLoadTimer;

	FName newSessionName;

	void DelayedLoad(int Port);

	bool AttemptAutoLogin();


	void LoginDisplayDelay();
	FTimerHandle LoginDisplayDelayHandle;

	class ARMainMenuController* MenuController;

	class IOnlineSubsystem* onlineSubsystem;
	IOnlineIdentityPtr identityPtr;
	IOnlineSessionPtr sessionPtr;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void CreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void FindSessionsCompleted(bool bWasSuccessful);
	void JoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type Result, int Port);

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> SelectLevel;

	FName SessionNameKey{ "SESSION_NAME" };

	void LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad);

	FName CurrentLobbyName;

	void CreateTestEOSSession();

	FTimerHandle FindCreatedSessionTimerHandle;
	FTimerHandle FindCreatedSessionTimeoutHandle;
	void StartFindCreatedSession(FGuid SessionUniqueId, int Port);
	void StopFindingCreatedSession();
	void TryFindCreatedSession(FGuid SessionUniqueId, int Port);
	void JoinCreatedSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type Result, int Port);
	void FindCreatedSessionResultsReceived(bool bWasSuccessful, FGuid SessionUniqueId, int Port);
	void FindCreatedSessionTimeoutReached();

	UPROPERTY(EditDefaultsOnly, Category = "Session Creation")
	float FindCreatedSessionTimeroutSeconds = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Session Creation")
	float FindCreatedSessionInterval = 1.f;

	FORCEINLINE FName GetLocalTestArgName() const { return FName{"LOCAL_TEST"}; }
	bool IsLocalTesting() const;

	/*****************************/
	/*         Session Server    */
	/*****************************/
	
public:
	void StartSession();
	void PlayerJoined(APlayerController* PlayerController);
	void PlayerLeft(AController* PlayerController);

private:
	FString DedicateServerSessionName;

	FTimerHandle NewSessionWiatClientTerminateHandle;

	TSet<FUniqueNetIdRepl> CurrentPlayerIds;


	void TerminateSessionServer();

	void CreateSessionBasedOnCLIArgs();
	FName SessionGUIDKey{"SESSION_GUID" };
	FORCEINLINE FName GetSessionGUIDNameKey() const { return SessionGUIDKey; }

	FString GetCoordinatorURLStr() const;
	FName SessionCoordinatorURLKey{"COORDINATOR_URL"};
	FORCEINLINE FName GetCoordinatorURLStrKey() const { return SessionCoordinatorURLKey; }

	void SesssionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bProcessdSuccessfully, FGuid SessionUniqueId);
	void EndSessionCompleted(FName EndedSessionName, bool bWasSuccessful);
};
