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
	void Server_CharacterSelected(URCharacterDefination* newSelectedCharacterDefination);
	
private:
	UPROPERTY(replicatedUsing = OnRep_SelectedCharacter)
	class URCharacterDefination* SelectedCharacter;

	UFUNCTION()
	void OnRep_SelectedCharacter();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
};
