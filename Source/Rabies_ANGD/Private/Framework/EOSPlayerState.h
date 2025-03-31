// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EOSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedCharacterReplicated, const class URCharacterDefination*, newPick);
/**
 * 
 */
UCLASS()
class AEOSPlayerState : public APlayerState
{
	GENERATED_BODY()

	// Character select

private:
	UPROPERTY(replicatedUsing = OnRep_PickedCharacter, VisibleAnywhere, Category = "Character")
	class URCharacterDefination* PickedCharacter;

	UFUNCTION()
	void OnRep_PickedCharacter();

public:
	FOnPickedCharacterReplicated OnPickedCharacterReplicated;

	UFUNCTION()
	class URCharacterDefination* GetCharacterDefination() const;
	 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_IssueCharacterPick(class URCharacterDefination* newPickedCharacterDefination);


	/** Copy properties which need to be saved in inactive PlayerState */
	virtual void CopyProperties(APlayerState* PlayerState) override;


public:
	// Gameplay
	AEOSPlayerState();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnPossessPlayer(class ARPlayerBase* myPlayer);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateHitscanRotator(FRotator newRot, FVector newLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateSocketLocations(FVector rootAimingLoc, FVector rangedLoc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ProcessDotFly(ARPlayerBase* player);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ProcessDotFlyingStamina(float newValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatePlayerVelocity(FVector velocity);

	UFUNCTION(Client, Reliable)
	void Server_RevivePlayer();

	UFUNCTION(Server, Reliable)
	void Server_CreateBossHealth(int level, class AREnemyBase* enemy);

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

	UFUNCTION()
	void OnRep_HitScanLocation();

	UPROPERTY(Replicated)
	FRotator hitscanRotation;

	UPROPERTY(replicatedUsing = OnRep_PlayerVelocity)
	FVector playerVelocity;

	UFUNCTION()
	void OnRep_PlayerVelocity();
	
	UPROPERTY(replicatedUsing = OnRep_HitScanLocation)
	FVector hitscanLocation;

	UPROPERTY(replicatedUsing = OnRep_DotFlyStamina)
	float dotFlyStamina = 1.0f;

	UFUNCTION()
	void OnRep_DotFlyStamina();

	UPROPERTY(Replicated)
	FVector RootAiming_SocketLocation;

	UPROPERTY(Replicated)
	FVector Ranged_SocketLocation;

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
