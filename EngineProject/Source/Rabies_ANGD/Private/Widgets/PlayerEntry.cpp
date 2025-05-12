// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerEntry.h"
#include "Components/TextBlock.h"
#include "Framework/EOSPlayerState.h"

void UPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	AEOSPlayerState* playerState = Cast<AEOSPlayerState>(ListItemObject);
	if (playerState)
	{
		playerNameCheck = playerState->GetPlayerName();
		PlayerName->SetText(FText::FromString(playerNameCheck));
	}
}

FString UPlayerEntry::GetPlayerName()
{
	return playerNameCheck;
}
