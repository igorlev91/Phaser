// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Enemy/REnemyBase.h"
#include "Character/RCharacterBase.h"
#include "Actors/ChestSpawnLocation.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"

void AEOSActionGameState::BeginPlay()
{
	// this will be on server side

    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChestSpawnLocation::StaticClass(), spawnLocations);

    if (spawnLocations.Num() <= AmountOfChests) return;

    for (int i = 0; i < AmountOfChests; i++)
    {
        float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
        SpawnChest(spawnLocations[randomSpawn]->GetActorLocation());
        spawnLocations.RemoveAt(randomSpawn);
    }

    float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
    SpawnEnemy(0, spawnLocations[randomSpawn]->GetActorLocation());
    spawnLocations.RemoveAt(randomSpawn);
}

void AEOSActionGameState::SpawnChest_Implementation(FVector SpawnLocation)
{
    if (HasAuthority() && ItemChestClass != nullptr) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        AllChests.Add(newChest); // make sure that the chest has bReplicates to true
        // You can do additional setup for NewActor if necessary
    }
}

void AEOSActionGameState::SpawnEnemy_Implementation(int EnemyIDToSpawn, FVector SpawnLocation)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AREnemyBase* newEnemy = GetWorld()->SpawnActor<AREnemyBase>(EnemyLibrary[EnemyIDToSpawn], SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        AllEnemies.Add(newEnemy); // make sure that the enemies has bReplicates to true
        // You can do additional setup for NewActor if necessary
    }
}

void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChests, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllEnemies, COND_None);
}