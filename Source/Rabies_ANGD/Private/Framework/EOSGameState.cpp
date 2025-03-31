// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RCharacterSelectController.h"
#include "Framework/RCharacterDefination.h"
#include "Actors/CagedCharacter.h"
#include "Actors/Clipboard.h"
#include "Algo/Sort.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Player/RCharacterSelectController.h"
#include "Actors/RRightButton.h"
#include "EOSGameState.h"
#include "EOSGameInstance.h"

void AEOSGameState::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().SetTimer(TryLoadIntoGameTimer, this, &AEOSGameState::TryLoadIntoGame, 3.0f, true);
}

URCharacterDefination* AEOSGameState::GetDefinationFromIndex(int index)
{
	return Characters[index];
}

void AEOSGameState::Server_ReadyUp_Implementation()
{
	ReadiedPlayers++;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Waiting for players..."));

	if (PlayerArray.Num() == ReadiedPlayers)
	{
		GetWorld()->GetTimerManager().SetTimer(TryReadupGameTimer, this, &AEOSGameState::LoadMapAndListen, 1.0f, false);
	}

}

void AEOSGameState::SetSessionName(const FName& updatedSessionName)
{
	SessionName = updatedSessionName;
}

const TArray<class URCharacterDefination*>& AEOSGameState::GetCharacters() const
{
	return Characters;
}

bool AEOSGameState::bCharcterSelected(const URCharacterDefination* CharacterToCheck) const
{
	return SelectedCharacters.Contains(CharacterToCheck);
}

void AEOSGameState::UpdateCharacterSelection(const URCharacterDefination* Selected, const URCharacterDefination* Deselected)
{
	if (!HasAuthority()) return;

	if (Selected != nullptr)
	{
		SelectedCharacters.Add(Selected);
	}

	if (Deselected)
	{
		SelectedCharacters.Remove(Deselected);
	}

	NetMulticast_UpdateCharacterSelection(Selected, Deselected);
}

void AEOSGameState::OnRep_SessionName()
{
	OnSessionNameReplicated.Broadcast(SessionName);
}

void AEOSGameState::OnRep_ReadiedPlayers()
{

}

void AEOSGameState::NetMulticast_UpdateCharacterSelection_Implementation(const URCharacterDefination* Selected, const URCharacterDefination* Deselected)
{
	OnCharacterSelectionReplicated.Broadcast(Selected, Deselected);
}

void AEOSGameState::LoadMapAndListen_Implementation()
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Loading into game"));
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

void AEOSGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSGameState, SessionName, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSGameState, ReadiedPlayers, COND_None, REPNOTIFY_Always);

}