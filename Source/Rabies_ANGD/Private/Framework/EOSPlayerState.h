// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EOSPlayerState.generated.h"

class URCharacterDefination;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedCharacterReplicated, const URCharacterDefination*, newSelection);
/**
 * 
 */
UCLASS()
class AEOSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AEOSPlayerState();
	
	FOnSelectedCharacterReplicated OnSelectedCharacterReplicated;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnPossessPlayer(class ARPlayerBase* myPlayer);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CharacterSelected(URCharacterDefination* newSelectedCharacterDefination);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateHitscanRotator(FRotator newRot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateSocketLocations(FVector rootAimingLoc, FVector rangedLoc);

	UFUNCTION()
	FRotator GetHitscanRotator() { return hitscanRotation; }

	UFUNCTION()
	FVector GetRootAimingLocation() { return RootAiming_SocketLocation; }

	UFUNCTION()
	FVector GetRangedLocation() { return Ranged_SocketLocation; }

	
private:
	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(replicatedUsing = OnRep_SelectedCharacter)
	class URCharacterDefination* SelectedCharacter;

	UFUNCTION()
	void OnRep_SelectedCharacter();

	UPROPERTY(Replicated)
	FRotator hitscanRotation;

	UPROPERTY(Replicated)
	FVector RootAiming_SocketLocation;

	UPROPERTY(Replicated)
	FVector Ranged_SocketLocation;

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
