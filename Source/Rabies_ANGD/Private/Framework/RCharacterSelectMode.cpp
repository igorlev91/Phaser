// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RCharacterSelectMode.h"
#include "Framework/RCharacterDefination.h"
#include "Actors/CagedCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Player/RCharacterSelectController.h"
#include "Algo/Sort.h"
#include "EOSGameState.h"
#include "EOSGameInstance.h"

void ARCharacterSelectMode::InitGameState()
{
	Super::InitGameState();

	AEOSGameState* gameState = Cast<AEOSGameState>(GameState);
	UEOSGameInstance* gameInstance = GetGameInstance<UEOSGameInstance>();

	if (gameState && gameInstance)
	{
		gameState->SetSessionName(gameInstance->GetCurrentSessionName());
	}

    GetCagedCharacters();
}

void ARCharacterSelectMode::GetCagedCharacters()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACagedCharacter::StaticClass(), FoundActors);
    CagedCharacters.Empty();

    for (AActor* Actor : FoundActors)
    {
        ACagedCharacter* CagedCharacter = Cast<ACagedCharacter>(Actor);
        if (CagedCharacter)
        {
            CagedCharacters.Add(CagedCharacter);
            UE_LOG(LogTemp, Warning, TEXT("CagedCharacter: %s"), *CagedCharacter->GetName());
        }
    }

    Algo::Sort(CagedCharacters, [](const ACagedCharacter* A, const ACagedCharacter* B)
        {
            return A->CharacterIndex < B->CharacterIndex; // Sort in ascending order
        });
}

URCharacterDefination* ARCharacterSelectMode::NextCharacter()
{
    CagedCharacters[0]->TickPosition(OffScreen, Sideline, true);

    if (CagedCharacters.Num() > 1) // Only rotate if there's more than one element
    {
        ACagedCharacter* FirstElement = CagedCharacters[0];
        CagedCharacters.RemoveAt(0);
        CagedCharacters.Add(FirstElement);
    }

    CagedCharacters[0]->TickPosition(ShownCage, Sideline, false);

    Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController())->SetCurrentlyHoveredCharacter(CagedCharacters[0]->Character);
    return CagedCharacters[0]->Character;
}
