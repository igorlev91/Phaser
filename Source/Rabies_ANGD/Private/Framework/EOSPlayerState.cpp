// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/EOSGameState.h"
#include "Framework/RCharacterDefination.h"
#include "Kismet/GameplayStatics.h"

AEOSPlayerState::AEOSPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;

}

void AEOSPlayerState::Server_CharacterSelected_Implementation(URCharacterDefination* newSelectedCharacterDefination)
{
	//On server
	if (newSelectedCharacterDefination == SelectedCharacter)
	{
		return; // you're chillin, already have that character
	}

	AEOSGameState* GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	GameState->UpdateCharacterSelection(newSelectedCharacterDefination, SelectedCharacter);
	SelectedCharacter = newSelectedCharacterDefination;
	OnSelectedCharacterReplicated.Broadcast(SelectedCharacter);
}

bool AEOSPlayerState::Server_CharacterSelected_Validate(URCharacterDefination* newSelectedCharacterDefination)
{
	return true;
}

void AEOSPlayerState::OnRep_SelectedCharacter()
{
	OnSelectedCharacterReplicated.Broadcast(SelectedCharacter);
}

void AEOSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, SelectedCharacter, COND_None, REPNOTIFY_Always)
}
