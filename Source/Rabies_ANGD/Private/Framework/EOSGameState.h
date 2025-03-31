// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EOSGameState.generated.h"


class URCharacterDefination;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionNameReplicated, const FName&, name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSelectionReplicated, const URCharacterDefination*, Selected, const URCharacterDefination*, Deslected);
/**
 * 
 */
UCLASS()
class AEOSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	FOnSessionNameReplicated OnSessionNameReplicated;
	FOnCharacterSelectionReplicated OnCharacterSelectionReplicated;

	FORCEINLINE FName GetSessionName() const { return SessionName; };

	void SetSessionName(const FName& updatedSessionName);

	const TArray<URCharacterDefination*>& GetCharacters() const;

	bool bCharcterSelected(const URCharacterDefination* CharacterToCheck) const;
	void UpdateCharacterSelection(const URCharacterDefination* Selected, const URCharacterDefination* Deselected);

	URCharacterDefination* GetDefinationFromIndex(int index);

	UFUNCTION(BlueprintCallable, Server, Reliable) // this function calls on both server and client if it's called from the server.
	void Server_ReadyUp();
	
private:

	UFUNCTION(NetMulticast, Reliable) // this function calls on both server and client if it's called from the server.
	void LoadMapAndListen();

	FTimerHandle TryReadupGameTimer;
	
	UPROPERTY(ReplicatedUsing = OnRep_SessionName)
	FName SessionName;

	UPROPERTY(ReplicatedUsing = OnRep_ReadiedPlayers)
	int ReadiedPlayers;

	UFUNCTION()
	void OnRep_SessionName();

	UFUNCTION()
	void OnRep_ReadiedPlayers();

	UFUNCTION(NetMulticast, Reliable) // this function calls on both server and client if it's called from the server.
	void NetMulticast_UpdateCharacterSelection(const URCharacterDefination* Selected, const URCharacterDefination* Deselected);

	UPROPERTY(EditDefaultsOnly)
	TArray<URCharacterDefination*> Characters;

	UPROPERTY(VisibleAnywhere)
	TArray<const URCharacterDefination*> SelectedCharacters;

	UPROPERTY()
	class AEOSPlayerState* PlayerState;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> GameLevel;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
