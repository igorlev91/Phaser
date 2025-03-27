// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Actors/ItemPickup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerState.h"
#include "Character/RCharacterBase.h"
#include "Actors/ChestSpawnLocation.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"
#include "Actors/EnemySpawnLocation.h"
#include "GameplayAbilities/GA_AbilityBase.h"

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

    WaveLevel = 0;
    WaveTime = enemyInitalSpawnRate;
    WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
}

void AEOSActionGameState::SpawnChest_Implementation(FVector SpawnLocation)
{
    if (HasAuthority() && ItemChestClass != nullptr) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        newChest->SetOwner(this);
        AllChests.Add(newChest); // make sure that the chest has bReplicates to true
    }
}

void AEOSActionGameState::SpawnEnemy_Implementation(int EnemyIDToSpawn, FVector SpawnLocation)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AREnemyBase* newEnemy = GetWorld()->SpawnActor<AREnemyBase>(EnemyLibrary[EnemyIDToSpawn], SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        newEnemy->SetOwner(this);
        UAbilitySystemComponent* ASC = newEnemy->GetAbilitySystemComponent();
        ASC->SetOwnerActor(newEnemy);
        newEnemy->InitLevel(WaveLevel);
        AllEnemies.Add(newEnemy); // make sure that the enemies has bReplicates to true
    }
}

void AEOSActionGameState::SelectEnemy(AREnemyBase* selectedEnemy)
{
    for (int i = 0; i < AllEnemies.Num(); i++)
    {
        if (selectedEnemy == AllEnemies[i])
        {
            RemoveEnemy(i);
            return;
        }
    }
}


void AEOSActionGameState::SelectChest(AItemChest* openedChest)
{
    for (int i = 0; i < AllChests.Num(); i++)
    {
        if (openedChest == AllChests[i])
        {
            OpenedChest(i);
            return;
        }
    }
}

void AEOSActionGameState::SelectItem(AItemPickup* selectedItem, ARPlayerBase* targetingPlayer)
{
    for (int i = 0; i < AllItems.Num(); i++)
    {
        if (selectedItem == AllItems[i])
        { 
            PickedUpItem(i, targetingPlayer);
            return;
        }
    }
}

void AEOSActionGameState::AwardEnemyKill_Implementation(TSubclassOf<class UGameplayEffect> rewardEffect)
{
    if (HasAuthority())
    {
        for (APlayerState* playerState : PlayerArray)
        {
            if (playerState && playerState->GetPawn())
            {
                ARPlayerBase* player = Cast<ARPlayerBase>(playerState->GetPawn());
                if (player)
                {
                    player->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(rewardEffect.GetDefaultObject(), 1.0f, player->GetAbilitySystemComponent()->MakeEffectContext());
                }
            }
        }
    }
}

void AEOSActionGameState::WaveSpawn(float timeToNextWave)
{
    if (timeToNextWave >= WaveTime)
    {
        WaveLevel += PlayerArray.Num();
        SpawnEnemyWave(1);
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
    }
    else
    {
        timeToNextWave += GetWorld()->GetDeltaSeconds();
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, timeToNextWave));
    }
}

void AEOSActionGameState::SpawnEnemyWave(int amountOfEnemies)
{
    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), spawnLocations);

    for (int i = 0; i < amountOfEnemies; i++)
    {
        float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
        SpawnEnemy(1, spawnLocations[randomSpawn]->GetActorLocation());
        spawnLocations.RemoveAt(randomSpawn);
    }
}

void AEOSActionGameState::RemoveEnemy_Implementation(int enemyID)
{
    if (HasAuthority())
    {
        AllEnemies[enemyID]->UpdateEnemyDeath();
        AllEnemies.RemoveAt(enemyID);
    }
}

void AEOSActionGameState::PickedUpItem_Implementation(int itemID, ARPlayerBase* targetingPlayer)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        targetingPlayer->AddNewItem(AllItems[itemID]->ItemAsset);
        AllItems[itemID]->UpdateItemPickedup();
        AllItems.RemoveAt(itemID);
    }
}

void AEOSActionGameState::OpenedChest_Implementation(int chestID)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        AllChests[chestID]->UpdateChestOpened();

        URItemDataAsset* newData = ItemLibrary[0];

        if (ItemSelection.IsEmpty() == false)
        {
            int32 randomIndex = FMath::RandRange(0, ItemSelection.Num() - 1);
            newData = ItemSelection[randomIndex];
        }
        else
        {
            int32 randomIndex = FMath::RandRange(0, ItemLibrary.Num() - 1);
            newData = ItemLibrary[randomIndex];
        }

        FActorSpawnParameters SpawnParams;
        AItemPickup* newitem = GetWorld()->SpawnActor<AItemPickup>(ItemPickupClass, AllChests[chestID]->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
        AllItems.Add(newitem); // make sure that the chest has bReplicates to true
        newitem->SetOwner(this);
        newitem->SetupItem(newData);
    }
}

void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChests, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllEnemies, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllItems, COND_None);
}