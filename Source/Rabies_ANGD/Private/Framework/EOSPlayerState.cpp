// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSGameState.h"
#include "Framework/RCharacterDefination.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEOSPlayerState::AEOSPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;

}

void AEOSPlayerState::Server_RevivePlayer_Implementation()
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetReviveTag(), eventData);

	Player->ServerSetPlayerReviveState(false);
	UE_LOG(LogTemp, Warning, TEXT("Getting revived"));
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

void AEOSPlayerState::Server_UpdateHitscanRotator_Implementation(FRotator newRot, FVector newLocation)
{
	//On server
	hitscanLocation = newLocation;
	hitscanRotation = newRot;
}

bool AEOSPlayerState::Server_UpdateHitscanRotator_Validate(FRotator newRot, FVector newLocation)
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

ARPlayerBase* AEOSPlayerState::GetPlayer()
{
	return Player;
}

void AEOSPlayerState::OnRep_SelectedCharacter()
{
	OnSelectedCharacterReplicated.Broadcast(SelectedCharacter);
}

void AEOSPlayerState::OnRep_HitScanLocation()
{
	if (Player == nullptr)
		return;
	Player->viewPivot->SetRelativeLocation(hitscanLocation);
}

void AEOSPlayerState::Server_ProcessDotFly_Implementation(ARPlayerBase* player)
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(player, URAbilityGenericTags::GetApplyGravityJump(), eventData);
	//UE_LOG(LogTemp, Error, TEXT("%s Gravity jump implementaiton"), *GetName());
}

bool AEOSPlayerState::Server_ProcessDotFly_Validate(ARPlayerBase* player)
{
	return true;
}

void AEOSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Player, COND_None);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, SelectedCharacter, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, hitscanRotation, COND_None);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, hitscanLocation, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Ranged_SocketLocation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, RootAiming_SocketLocation, COND_None);
}
