// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RCharacterSelectMode.generated.h"


class URCharacterDefination;
/**
 * 
 */
UCLASS()
class ARCharacterSelectMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void InitGameState() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector ShownCage;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector OffScreen;

	UPROPERTY(EditDefaultsOnly, Category = "CagedCharacter")
	FVector Sideline;

	UPROPERTY(VisibleAnywhere, Category = "CagedCharacter")
	TArray<class ACagedCharacter*> CagedCharacters;

	void GetCagedCharacters();

public:

	UFUNCTION()
	URCharacterDefination* NextCharacter();
};
