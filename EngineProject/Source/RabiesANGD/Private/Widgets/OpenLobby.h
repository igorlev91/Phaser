// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpenLobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyEntrySelected, int, EntryLobbyIndex);
/**
 *
 */
UCLASS()
class UOpenLobby : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitLobbyEntry(const FName& LobbyName, int EntryLobbyIndex);

	FOnLobbyEntrySelected OnLobbyEntrySelected;

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* LobbyButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LobbyPlayerLimitText;

	UFUNCTION()
	void LobbyButtonClicked();

	UFUNCTION()
	void LobbyButtonAudio();

	int LobbyIndex = -1;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* HoverAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ClickAudio;

	UFUNCTION()
	void PlayHoverAudio();

protected:

	virtual void NativeConstruct() override;

};
