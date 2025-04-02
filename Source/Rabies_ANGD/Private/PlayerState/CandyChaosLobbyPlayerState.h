// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Widgets/Lobby/HeroeCard/HeroeCard.h"
#include "CandyChaosLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class  ACandyChaosLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

	ACandyChaosLobbyPlayerState();

private:
	TArray<UHeroeCard*> HeroesCards;
	
};
