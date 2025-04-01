// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "ConnectOnlineMenu.generated.h"

/**
 * 
 */
UCLASS()
class UConnectOnlineMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	class UEOSGameInstance* GameInst;

private:

	UPROPERTY(meta = (BindWidget))
	class URButton* ReturnBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* CreateSessionBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* FindSessionsBtn;

	UPROPERTY(meta = (BindWidget))
	class URButton* JoinLobbyBtn;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* SessionNameText;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* LobbyListScrollBox;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<class UOpenLobby> OnlineLobbyClass;

	UFUNCTION()
	void SessionNameTextChanged(const FText& NewText);

	UFUNCTION()
	void CreateSessionButtonClicked();

	UFUNCTION()
	void FindSessionsButtonClicked();

	void SessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& searchResults);

	UFUNCTION()
	void LobbySelected(int lobbyIndex);

	UFUNCTION()
	void Return();

	UFUNCTION()
	void JoinLobbyButtonClicked();

	int SelectedLobbyIndex = -1;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* HoverAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ClickAudio;

	UFUNCTION()
	void PlayHoverAudio();

	UFUNCTION()
	void JoinLobby();
};
