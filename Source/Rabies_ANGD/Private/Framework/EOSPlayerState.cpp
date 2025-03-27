// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSGameState.h"
#include "Framework/RCharacterDefination.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEOSPlayerState::AEOSPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;

}

void AEOSPlayerState::Server_OnPossessPlayer_Implementation(ARPlayerBase* myPlayer)
{
	Player = myPlayer;
}

bool AEOSPlayerState::Server_OnPossessPlayer_Validate(ARPlayerBase* myPlayer)
{
	return true;
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

void AEOSPlayerState::Server_UpdateHitscanRotator_Implementation(FRotator newRot)
{
	//On server
	hitscanRotation = newRot;
}

bool AEOSPlayerState::Server_UpdateHitscanRotator_Validate(FRotator newRot)
{
	return true;
}

void AEOSPlayerState::Server_UpdateSocketLocations_Implementation(FVector rootAimingLoc, FVector rangedLoc)
{
	//On server
	RootAiming_SocketLocation = rootAimingLoc;
	Ranged_SocketLocation = rangedLoc;
}

bool AEOSPlayerState::Server_UpdateSocketLocations_Validate(FVector rootAimingLoc, FVector rangedLoc)
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

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, SelectedCharacter, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, hitscanRotation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, Ranged_SocketLocation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, RootAiming_SocketLocation, COND_None);
}
