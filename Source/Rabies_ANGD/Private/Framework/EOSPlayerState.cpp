// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSGameState.h"
#include "Player/RPlayerController.h"
#include "Framework/RCharacterDefination.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

void AEOSPlayerState::OnRep_PickedCharacter()
{
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

void AEOSPlayerState::OnRep_HoveredCharacterIndex()
{
	OnHoveredCharacterIndexReplicated.Broadcast(HoveredCharacterIndex);
}

URCharacterDefination* AEOSPlayerState::GetCharacterDefination() const
{
	return PickedCharacter;
}

void AEOSPlayerState::Server_IssueCharacterPick_Implementation(URCharacterDefination* newPickedCharacterDefination)
{
	if (newPickedCharacterDefination == PickedCharacter)
		return;

	AEOSGameState* gameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!gameState)
		return;

	gameState->UpdateCharacterSelection(newPickedCharacterDefination, PickedCharacter);
	PickedCharacter = newPickedCharacterDefination;
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

bool AEOSPlayerState::Server_IssueCharacterPick_Validate(URCharacterDefination* newPickedCharacterDefination)
{
	return true;
}

void AEOSPlayerState::Server_ChangeHoveredCharacterPick_Implementation()
{
	HoveredCharacterIndex++;

	if (HoveredCharacterIndex == 4)
		HoveredCharacterIndex = 0;

	OnHoveredCharacterIndexReplicated.Broadcast(HoveredCharacterIndex);
}

bool AEOSPlayerState::Server_ChangeHoveredCharacterPick_Validate()
{
	return true;
}

void AEOSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Copied Updated Character"));
	AEOSPlayerState* NewPlayerState = Cast<AEOSPlayerState>(PlayerState);
	NewPlayerState->PickedCharacter = PickedCharacter;
}

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

void AEOSPlayerState::Server_CreateBossHealth_Implementation(int level, class AREnemyBase* enemy)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Has Auth"));
		if (Player == nullptr)
			return;

		UE_LOG(LogTemp, Error, TEXT("Getting in"));
		Player->playerController->AddBossEnemy(level, enemy);
	}
}

ARPlayerBase* AEOSPlayerState::GetPlayer()
{
	return Player;
}

void AEOSPlayerState::OnRep_HitScanLocation()
{
	if (Player == nullptr)
		return;

	Player->viewPivot->SetRelativeLocation(hitscanLocation);
}

void AEOSPlayerState::OnRep_PlayerVelocity()
{
	Player->PlayerVelocity = playerVelocity;
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

void AEOSPlayerState::Server_ProcessDotFlyingStamina_Implementation(float newValue)
{
	if (Player == nullptr)
		return;

	dotFlyStamina = newValue;
	Player->DotFlyStamina = dotFlyStamina;
}

bool AEOSPlayerState::Server_ProcessDotFlyingStamina_Validate(float newValue)
{
	return true;
}

void AEOSPlayerState::OnRep_DotFlyStamina()
{
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Getting player"), *GetName());
		Player = Cast<ARPlayerBase>(GetPawn());
		return;
	}

	Player->DotFlyStamina = dotFlyStamina;
}

void AEOSPlayerState::Server_UpdatePlayerVelocity_Implementation(FVector velocity)
{
	if (Player == nullptr)
		return;

	playerVelocity = velocity;
	Player->PlayerVelocity = playerVelocity;
}

bool AEOSPlayerState::Server_UpdatePlayerVelocity_Validate(FVector velocity)
{
	return true;
}


void AEOSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Player, COND_None);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, PickedCharacter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, HoveredCharacterIndex, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, playerVelocity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, dotFlyStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, hitscanRotation, COND_None);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, hitscanLocation, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Ranged_SocketLocation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, RootAiming_SocketLocation, COND_None);
}