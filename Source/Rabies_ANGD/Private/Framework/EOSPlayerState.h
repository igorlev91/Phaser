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
	void Server_UpdateHitscanRotator(FRotator newRot, FVector newLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateSocketLocations(FVector rootAimingLoc, FVector rangedLoc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ProcessDotFly(ARPlayerBase* player);

	UFUNCTION(NetMulticast, Reliable)
	void Server_RevivePlayer();

	UFUNCTION()
	FRotator GetHitscanRotator() { return hitscanRotation; }

	UFUNCTION()
	FVector GetRootAimingLocation() { return RootAiming_SocketLocation; }

	UFUNCTION()
	FVector GetRangedLocation() { return Ranged_SocketLocation; }

	UFUNCTION()
	class ARPlayerBase* GetPlayer();

	
private:
	UPROPERTY(Replicated)
	class ARPlayerBase* Player;

	UPROPERTY(replicatedUsing = OnRep_SelectedCharacter)
	class URCharacterDefination* SelectedCharacter;

	UFUNCTION()
	void OnRep_SelectedCharacter();

	UFUNCTION()
	void OnRep_HitScanLocation();

	UPROPERTY(Replicated)
	FRotator hitscanRotation;
	
	UPROPERTY(replicatedUsing = OnRep_HitScanLocation)
	FVector hitscanLocation;

	UPROPERTY(Replicated)
	FVector RootAiming_SocketLocation;

	UPROPERTY(Replicated)
	FVector Ranged_SocketLocation;

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
