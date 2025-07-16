// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EOSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedCharacterReplicated, const class URCharacterDefination*, newPick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredCharacterIndexReplicated, int, newIndex);
DECLARE_MULTICAST_DELEGATE(FOnLoadNewScene);

UENUM(BlueprintType)
enum class ESettingsType : uint8
{
	Easy,
	Medium,
	Hard,
	Lunitic
};
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

	UFUNCTION()
	void OnRep_HoveredCharacterIndex();

	UPROPERTY(Replicated)
	ESettingsType GameSettings = ESettingsType::Medium;

	UPROPERTY(Replicated)
	bool bToyBox = false;

	UPROPERTY(Replicated)
	bool bOwnsSettings = false;

	UPROPERTY(Replicated)
	bool bSpareParts = false;

public:
	FOnPickedCharacterReplicated OnPickedCharacterReplicated;
	FOnHoveredCharacterIndexReplicated OnHoveredCharacterIndexReplicated;
	FOnLoadNewScene OnLoadNewScene;

	UPROPERTY(replicatedUsing = OnRep_HoveredCharacterIndex, VisibleAnywhere, Category = "Character")
	int HoveredCharacterIndex = 0;

	UFUNCTION()
	class URCharacterDefination* GetCharacterDefination() const;
	 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_IssueCharacterPick(class URCharacterDefination* newPickedCharacterDefination);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangeHoveredCharacterPick();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetToyBox(bool state);

	UFUNCTION(Server, Reliable, WithValidation)
	void SeOwnsSettings(bool state);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetSpareParts(bool state);

	UFUNCTION()
	bool GetToyBox() { return bToyBox; }

	UFUNCTION()
	bool GetOwnsSettings() { return bOwnsSettings; }

	UFUNCTION()
	bool GetSpareParts() { return bSpareParts; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetSettings(ESettingsType settings);

	UFUNCTION()
	ESettingsType GetSettings() { return GameSettings; }

	/** Copy properties which need to be saved in inactive PlayerState */
	virtual void CopyProperties(APlayerState* PlayerState) override;


public:
	// Gameplay
	AEOSPlayerState();


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnPossessPlayer(class ARPlayerBase* myPlayer);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateHitscanRotator(FRotator newRot, FVector newLocation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Server_UpdateCameraShake(FVector newLoc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateSocketLocations(FVector rootAimingLoc, FVector rangedLoc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateAlternateAiming(FVector rangedLoc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ProcessDotFly(ARPlayerBase* player);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ProcessDotFlyingStamina(float newValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdatePlayerVelocity(FVector velocity);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateHoldingUltimate(bool state);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateDotCenterLocation(FVector newLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_WonTheGame(const FString& characterName);

	UFUNCTION(Client, Reliable)
	void Server_RevivePlayer();

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_Load();

	UFUNCTION()
	void Client_DelayLoad();

	UFUNCTION(Server, Reliable)
	void Server_CreateBossHealth(int level, class AREnemyBase* enemy);


	UFUNCTION()
	FVector GetShakeLocation() { return cameraShakeLocation; }

	UFUNCTION()
	bool GetHoldingUltimate() { return bHoldingUltimate; }

	UFUNCTION()
	FRotator GetHitscanRotator() { return hitscanRotation; }

	UFUNCTION()
	FVector GetRootAimingLocation() { return RootAiming_SocketLocation; }

	UFUNCTION()
	FVector GetAltRangedLocation() { return AltRanged_SocketLocation; }

	UFUNCTION()
	void SetFullyDead(bool state, ARPlayerBase* player);

	UFUNCTION()
	void SetStickDot(bool state, ARPlayerBase* dot);

	UFUNCTION()
	FVector GetRangedLocation() { return Ranged_SocketLocation; }

	UFUNCTION()
	class ARPlayerBase* GetPlayer();

	
private:
	FTimerHandle DotHandle;

	UPROPERTY(Replicated)
	class ARPlayerBase* Player;

	UFUNCTION()
	void OnRep_HitScanLocation();

	UPROPERTY(Replicated)
	FRotator hitscanRotation;

	UPROPERTY(replicatedUsing = OnRep_CameraShake)
	FVector cameraShakeLocation;

	UFUNCTION()
	void OnRep_CameraShake();

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

	UPROPERTY(Replicated)
	FVector AltRanged_SocketLocation;

	UPROPERTY(Replicated)
	bool bHoldingUltimate;

	UPROPERTY(replicatedUsing = OnRep_FullyDead)
	bool bFullyDead;

	UFUNCTION()
	void OnRep_FullyDead();

	UPROPERTY(replicatedUsing = OnRep_DotCenterLocation)
	FVector DotCenterLocation;

	UFUNCTION()
	void OnRep_DotCenterLocation();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
