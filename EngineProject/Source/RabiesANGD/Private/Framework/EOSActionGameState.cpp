// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DisplayDebugHelpers.h"
#include "Actors/EscapeToWin.h"
#include "Actors/ItemPickup.h"
#include "Character/RCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/PlayerState.h"
#include "Actors/ChestSpawnLocation.h"
#include "Framework/EOSPlayerState.h"
#include "Player/RPlayerController.h"
#include "Actors/EnemySpawnLocation.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Framework/RItemDataAsset.h"
#include "Actors/PingActor.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "NiagaraFunctionLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Widgets/ReviveUI.h"
#include "Widgets/HealthBar.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Components/CapsuleComponent.h"
#include "Framework/EOSGameInstance.h"
#include "Components/AudioComponent.h"
#include "Framework/RSaveGame.h"

void AEOSActionGameState::BeginPlay()
{
    Super::BeginPlay();
	// this will be on server side

    if (HasAuthority() == false)
        return;

    FTimerHandle DelayStart;
    GetWorldTimerManager().SetTimer(DelayStart, this, &AEOSActionGameState::STARTGAME, 1.5f, false);
}

void AEOSActionGameState::ChooseDifficulty(ESettingsType settingsType)
{
    GameSettings = settingsType;

    switch (settingsType)
    {
    case ESettingsType::Easy:
        IncreasedPower = 0;
        enemyInitalSpawnRate = 10;
        enemySpawnRateChange = 5;
        enemyMax = 5;
        WaveSpawnCap = 3;
        break;

    case ESettingsType::Medium:
        IncreasedPower = 0;
        enemyInitalSpawnRate = 6;
        enemySpawnRateChange = 2;
        enemyMax = 30;
        WaveSpawnCap = 6;
        break;

    case ESettingsType::Hard:
        IncreasedPower = 1;
        enemyInitalSpawnRate = 4;
        enemySpawnRateChange = 1;
        enemyMax = 50;
        WaveSpawnCap = 12;
        break;

    case ESettingsType::Lunitic:
        IncreasedPower = 2;
        enemyInitalSpawnRate = 5;
        enemySpawnRateChange = 0;
        enemyMax = 80;
        WaveSpawnCap = 15;
        break;

    }
}

void AEOSActionGameState::STARTGAME()
{
    /*if (!IsValid(this))
    {
        UE_LOG(LogTemp, Error, TEXT("GameState is invalid during InitGameState"));
        return;
    }*/

    if (HasAuthority() == false)
        return;

    if (PlayerArray.Num() <= 0)
        return;

    Multicast_PlayAudio();

    if (devDiff == false)
    {
        bToyBox = false;
        bEnemyItemDrops = false;
        ChooseDifficulty(ESettingsType::Medium);
    }

    for (APlayerState* playerState : PlayerArray)
    {
        if (playerState)
        {
            AEOSPlayerState* eosPlayerState = Cast<AEOSPlayerState>(playerState);
            if (eosPlayerState)
            {
                if (eosPlayerState->GetOwnsSettings())
                {
                    if (devDiff == false)
                    {
                        bToyBox = eosPlayerState->GetToyBox();
                        bEnemyItemDrops = eosPlayerState->GetSpareParts();
                        ChooseDifficulty(eosPlayerState->GetSettings());
                    }
                }
            }
        }
    }


    OpenedChests = 2.0f;

    TArray<AActor*> EscapeToWinObj;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEscapeToWin::StaticClass(), EscapeToWinObj);
    EscapeToWin = Cast<AEscapeToWin>(EscapeToWinObj[0]);
    if (EscapeToWin)
    {
        EscapeToWin->SetOwner(this);
    }

    if (bEnemyItemDrops == false)
    {
        TArray<AActor*> spawnLocations;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChestSpawnLocation::StaticClass(), spawnLocations);
        for (AActor* spawnLoc : spawnLocations)
        {
            AChestSpawnLocation* chestSpawn = Cast<AChestSpawnLocation>(spawnLoc);
            if (chestSpawn)
            {
                chestSpawn->SetOwner(this);
                AllChestsSpawnLocations.Add(chestSpawn);
            }
        }

        MaxChests = AmountOfChests;

        for (int i = 0; i < AmountOfChests; i++)
        {
            float randomItemCost = FMath::RandRange(3, 10);
            SpawnChest(randomItemCost);
        }

        GetWorldTimerManager().SetTimer(ChestSpawnHandle, this, &AEOSActionGameState::PeriodicSpawnChest, ChestSpawnRate, true);
    }
    else
    {
        MaxChests = 1.0f;
    }

    TArray<AActor*> enemySpawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), enemySpawnLocations);

    WaveSpawnCap = FMath::Clamp(WaveSpawnCap, 1, enemySpawnLocations.Num());

    WaveLevel = IncreasedPower;
    WaveTime = enemyInitalSpawnRate;
    WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
}

void AEOSActionGameState::Multicast_PlayAudio_Implementation()
{
    if (MainAudio && BossAudio)
    {
        MainAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), MainAudio, 1.0f, 1.0f, 0.0f);
        BossComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BossAudio, 1.0f, 1.0f, 0.0f);

        if (BossComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("%s Recieved BossComponent Audios"), *GetName());
            BossComponent->SetVolumeMultiplier(0.0f);
        }
    }
}

void AEOSActionGameState::CrossfadeMusic_Implementation()
{
    if (MainAudioComponent == nullptr)
        return;

    if (BossComponent == nullptr)
        return;

    float mainAudioSound = MainAudioComponent->VolumeMultiplier;
    float bossAudioSound = BossComponent->VolumeMultiplier;

    BossComponent->SetVolumeMultiplier(FMath::Lerp(bossAudioSound, 1.0f, 2 * GetWorld()->GetDeltaSeconds()));
    MainAudioComponent->SetVolumeMultiplier(FMath::Lerp(mainAudioSound, 0.0f, 2 * GetWorld()->GetDeltaSeconds()));

    if (mainAudioSound <= 0.2f)
    {
        BossComponent->SetVolumeMultiplier(1);
        MainAudioComponent->SetVolumeMultiplier(0);
    }
    else
    {
        FTimerHandle crossFade;
        crossFade = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::CrossfadeMusic));
    }
}

void AEOSActionGameState::PeriodicSpawnChest_Implementation()
{
    float randomItemCost = FMath::RandRange(3, 10);
    SpawnChest(randomItemCost);
}

void AEOSActionGameState::SpawnChest_Implementation(float randomItemCost)
{
    if (HasAuthority() && ItemChestClass != nullptr && GetWorld() != nullptr) // Ensure we're on the server
    {
        if (AllChestsSpawnLocations.Num() <= AllChests.Num())
            return;

        float randomSpawn = FMath::RandRange(0, AllChestsSpawnLocations.Num() - 1);
        while (AllChestsSpawnLocations[randomSpawn]->OwningChest != nullptr)
        {
            randomSpawn = FMath::RandRange(0, AllChestsSpawnLocations.Num() - 1);
        }

        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, AllChestsSpawnLocations[randomSpawn]->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
        newChest->SetOwner(this);
        if (AllChestsSpawnLocations[randomSpawn]->bRareChest)
        {
            newChest->ScrapPrice = 80.0f;
        }
        else
        {
            newChest->ScrapPrice = randomItemCost * 5.0f;
        }
        newChest->SetUpUI(false);
        AllChestsSpawnLocations[randomSpawn]->OwningChest = newChest;
        AllChests.Add(newChest); // make sure that the chest has bReplicates to true
    }
}

void AEOSActionGameState::SpawnEnemy_Implementation(int EnemyIDToSpawn, FVector SpawnLocation, bool fromEnemySelection)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AREnemyBase* newEnemy = GetWorld()->SpawnActor<AREnemyBase>((fromEnemySelection) ? EnemySelection[EnemyIDToSpawn] : EnemyLibrary[EnemyIDToSpawn], SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (newEnemy == nullptr || this == nullptr)
        {
            
            return;
        }
        newEnemy->SetOwner(this);             
        UAbilitySystemComponent* ASC = newEnemy->GetAbilitySystemComponent();
        ASC->SetOwnerActor(newEnemy);
        newEnemy->InitLevel(WaveLevel);
        AllEnemies.Add(newEnemy); // make sure that the enemies has bReplicates to true
    }
}

void AEOSActionGameState::SelectEnemy(AREnemyBase* selectedEnemy, bool isDeadlock, bool bIsDeadlockComponent)
{
    if(isDeadlock)
        deadlockPos = selectedEnemy->GetActorLocation();

    if (bIsDeadlockComponent)
        EscapeToWin->UpdateEscapeToWin();

    for (int i = 0; i < AllEnemies.Num(); i++)
    {
        if (selectedEnemy == AllEnemies[i])
        {
            RemoveEnemy(i);
            break;
        }
    }

    if (isDeadlock)
    {
        StartBossFight(1);
        StartBossFight(2);
    }
}


void AEOSActionGameState::SelectChest(AItemChest* openedChest, int spawnCount)
{
    for (int i = 0; i < AllChests.Num(); i++)
    {
        if (openedChest == AllChests[i])
        {
            OpenedChest(i, spawnCount);
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

void AEOSActionGameState::LeaveLevel_Implementation()
{
    if (HasAuthority())
    {
        if (GameSettings == ESettingsType::Hard || GameSettings == ESettingsType::Lunitic)
        {
            for (APlayerState* playerState : PlayerArray)
            {
                AEOSPlayerState* eosPlayerState = Cast<AEOSPlayerState>(playerState);
                if (eosPlayerState)
                {
                    eosPlayerState->Server_WonTheGame(eosPlayerState->GetPawn()->GetName());
                }
            }
        }

    }

    LoadMapAndListen(CinematicEnding);
}


bool AEOSActionGameState::LeaveLevel_Validate()
{
    return true;
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
        //int enemiesToSpawn = WaveLevel * 3;
        //enemiesToSpawn = FMath::Clamp(enemiesToSpawn, 3, 6);
        SpawnEnemyWave(WaveLevel);
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
    }
    else
    {
        timeToNextWave += GetWorld()->GetDeltaSeconds();
        WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, timeToNextWave));
    }
}

void AEOSActionGameState::SpawnEnemyWave(int powerLevel)
{
    if (GetWorld() == nullptr)
        return;

    WaveTime += enemySpawnRateChange;
    WaveTime = FMath::Clamp(WaveTime, 2, 20);

    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), spawnLocations);

    int spawnsThisWave = 0;
    float power = powerLevel;

    while (power > 0 && spawnsThisWave < WaveSpawnCap)
    {
        if (AllEnemies.Num() >= enemyMax)
            return;

        //UE_LOG(LogTemp, Warning, TEXT("Power is: %f"), power);

        int enemyID = -1; // Null default, proceed with caution
        int tries = 0;
        while (enemyID < 0)
        {
            tries++;
            int randomEnemyID = FMath::RandRange(0, EnemySelection.Num() - 1);
            if (power >= EnemyIndexPowerLevels[randomEnemyID])
            {
                enemyID = randomEnemyID;
                power -= EnemyIndexPowerLevels[randomEnemyID];
            }
            else if(tries >= 20)
            {
                return;
            }
        }

        float randomPlayer = FMath::RandRange(0, PlayerArray.Num() - 1);
        if (PlayerArray[randomPlayer] == nullptr)
            return;

        if (PlayerArray[randomPlayer]->GetPawn() == nullptr)
            return;

        float randomSpawn = 0;
        float closestDistance = TNumericLimits<float>::Max();
        for (int i = 0; i < spawnLocations.Num(); i++)
        {
            if (spawnLocations[i] == nullptr)
            {
                continue;
            }

            float distance = FVector::Dist(spawnLocations[i]->GetActorLocation(), PlayerArray[randomPlayer]->GetPawn()->GetActorLocation());
            if (distance < closestDistance)
            {
                closestDistance = distance;
                randomSpawn = i;
            }
        }

        FTimerDelegate TimerDelegate;
        if (enemyID == 4)
        {
            TimerDelegate = FTimerDelegate::CreateUObject(this, &AEOSActionGameState::StartBossFight, 0);
        }
        else
        {
            TimerDelegate = FTimerDelegate::CreateUObject(this, &AEOSActionGameState::SpawnEnemy, enemyID, spawnLocations[randomSpawn]->GetActorLocation(), true);
        }

        FTimerHandle newSpawnHandle;
        float SpawnDelay = 0.1f * spawnsThisWave;
        GetWorldTimerManager().SetTimer(newSpawnHandle, TimerDelegate, SpawnDelay, false);
        spawnsThisWave++;
        spawnLocations.RemoveAt(randomSpawn);
    }
}

void AEOSActionGameState::StartBossFight_Implementation(int enemyID)
{
    if (enemyID == 1)
        deadlockPos.X += 300;

    if (enemyID == 0)
    {
        deadlockPos = FVector(0, 0, 3000);

        FTimerHandle crossFade;
        crossFade = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::CrossfadeMusic));
        if (DeadlockSpawnAudio != nullptr && HasAuthority())
        {
            FVector Location = FVector(0, 0, 1000);
            FRotator Rotation = FRotator::ZeroRotator;
            USoundAttenuation* Attenuation = nullptr;


            Multicast_RequestPlayAudio(nullptr, DeadlockSpawnAudio, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
        }
    }

    SpawnEnemy(enemyID, deadlockPos, false);
    
    for (APlayerState* playerState : PlayerArray)
    {
        if (playerState/* && playerState->GetOwningController()*/)
        {
            Cast<AEOSPlayerState>(playerState)->Server_CreateBossHealth(WaveLevel, AllEnemies.Last());
        }
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


void AEOSActionGameState::SpawnDeathItem_Implementation(AREnemyBase* enemyDropping, float propability, int itemsToDrop)
{
    if (bEnemyItemDrops == false)
    {
        return;
    }

    float randomDropChance = FMath::RandRange(0, 100);
    if (randomDropChance < propability)
    {
        int itemDropChance = propability + 20.0f;
        SpawnItem(enemyDropping->GetActorLocation(), itemDropChance, itemsToDrop);
    }
}

void AEOSActionGameState::PickedUpItem_Implementation(int itemID, ARPlayerBase* targetingPlayer)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        if (targetingPlayer == nullptr)
            return;

        if (AllItems.Num() <= 0)
            return;

        if (AllItems.IsValidIndex(itemID) == false)
            return;

        if (AllItems[itemID]->ItemAsset == KeyCard)
        {
            for (APlayerState* playerState : PlayerArray)
            {
                if (playerState && playerState->GetPawn())
                {
                    ARPlayerBase* player = Cast<ARPlayerBase>(playerState->GetPawn());
                    if (player)
                    {
                        player->PlayVoiceLine(player->ItemPickupSounds, 100);
                        player->AddNewItem(AllItems[itemID]->ItemAsset);
                    }
                }
            }
            if (EscapeToWin != nullptr)
            {
                Server_Ping(FVector(0,0,0), EscapeToWin);
            }
        }
        else
        {
            if (targetingPlayer->BigStickSounds.Num() > 0 && AllItems[itemID]->ItemAsset->ItemName == "Big Stick")
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->BigStickSounds, 100);
            }
            else if (targetingPlayer->SprayBottleSounds.Num() > 0 && AllItems[itemID]->ItemAsset->ItemName == "Spray Bottle")
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->SprayBottleSounds, 100);
            }
            else if (targetingPlayer->BagofNailsSounds.Num() > 0 && AllItems[itemID]->ItemAsset->ItemName == "Bag of Nails")
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->BagofNailsSounds, 100);
            }
            else if (targetingPlayer->AmmoSounds.Num() > 0 && AllItems[itemID]->ItemAsset->ItemName == "Ammo Box")
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->AmmoSounds, 100);
            }
            else if (targetingPlayer->FriendshipSounds.Num() > 0 && AllItems[itemID]->ItemAsset->ItemName == "Friendship Bracelet")
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->FriendshipSounds, 100);
            }
            else
            {
                targetingPlayer->PlayVoiceLine(targetingPlayer->ItemPickupSounds, 100);
            }


            Multicast_SpawnItemOnPlayer(AllItems[itemID]->ItemAsset, targetingPlayer);
            targetingPlayer->AddNewItem(AllItems[itemID]->ItemAsset);
        }
        AllItems[itemID]->UpdateItemPickedup();
        AllItems.RemoveAt(itemID);
    }
}

void AEOSActionGameState::Multicast_SpawnItemOnPlayer_Implementation(URItemDataAsset* itemData, ARPlayerBase* targetingPlayer)
{
    if (itemData == nullptr)
        return;
    
    if (targetingPlayer == nullptr)
        return;

    if (targetingPlayer->playerController)
    {
        if (targetingPlayer->playerController->IsItemUnique(itemData) == false)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Picked up dup item, not making another item"));
            return;
        }
    }

    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* newItemMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (newItemMesh)
    {
        if (targetingPlayer->bMiniLock == true)
        {
            newItemMesh->SetActorScale3D(FVector(5,5,5));
        }
        //newItemMesh->SetReplicates(true);
        UStaticMeshComponent* MeshComp = newItemMesh->GetStaticMeshComponent();
        if (MeshComp)
        {
            MeshComp->SetMobility(EComponentMobility::Movable);
            newItemMesh->SetMobility(EComponentMobility::Movable);
            MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            MeshComp->SetStaticMesh(itemData->ItemPlayerMesh);

            if (targetingPlayer->bBolthead)
            {
                FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
                MeshComp->AttachToComponent(targetingPlayer->GetItemAttachmentMesh(itemData->ItemName), AttachmentRules, itemData->ItemName);
                return;
            }
            
            FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
            MeshComp->AttachToComponent(targetingPlayer->GetMesh(), AttachmentRules, itemData->ItemName);
        }
    }
}


void AEOSActionGameState::OpenedChest_Implementation(int chestID, int spawnCount)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        AllChests[chestID]->UpdateChestOpened();

        SpawnItem(AllChests[chestID]->GetActorLocation(), AllChests[chestID]->ScrapPrice, spawnCount);
        OpenedChests++;
        FTimerHandle deleteTimerHandle;
        deleteTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::DeleteChest, chestID, 5.0f));
    }
}

void AEOSActionGameState::SpawnItem(FVector SpawnLocation, float cost, int spawnCount)
{
    for (int i = 0; i < spawnCount; i++)
    {
        URItemDataAsset* newData = ItemLibrary[0];
        TArray<URItemDataAsset*> ChosenLibrary = (ItemSelection.IsEmpty() == false) ? ItemSelection : ItemLibrary;
        if (ChosenLibrary == ItemLibrary && PlayerArray.Num() <= 1)
        {
            ChosenLibrary = SinglePlayerItemLibrary;
        }

        bool bRareItem = false;

        float randomItemRariety = FMath::RandRange(0, 75);
        if (cost > randomItemRariety)
        {
            float reRollRandomItemRariety = FMath::RandRange(0, 100);
            if (cost > reRollRandomItemRariety || cost >= 80)
            {
                bRareItem = true;
                ChosenLibrary = (bToyBox) ? ToyBoxRare : GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Rare);
            }
            else
            {
                ChosenLibrary = (bToyBox) ? ToyBoxUncommon : GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Uncommon);
            }
        }
        else
        {
            ChosenLibrary = (bToyBox) ? ToyBoxCommon : GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Common);
        }

        int32 randomIndex = FMath::RandRange(0, ChosenLibrary.Num() - 1);
        newData = ChosenLibrary[randomIndex];

        if (bRareItem == false)
        {
            if (GetKeyCard())
            {
                newData = KeyCard;
            }
        }

        if (bInstantKeyCard)
        {
            newData = KeyCard;
            bInstantKeyCard = false;
        }

        FActorSpawnParameters SpawnParams;
        FVector spawnAdjustment = SpawnLocation;
        spawnAdjustment.Z += 40;
        AItemPickup* newitem = GetWorld()->SpawnActor<AItemPickup>(ItemPickupClass, spawnAdjustment, FRotator::ZeroRotator, SpawnParams);
        AllItems.Add(newitem); // make sure that the chest has bReplicates to true]
        newitem->SetOwner(this);
        float x = FMath::RandRange(-100, 100);
        float y = FMath::RandRange(-100, 100);
        FVector ForceDirection = FVector(x, y, 600.f); // Adjust the Z value to set how much it pops up
        newitem->SetOwner(this);
        newitem->SetupItem(newData, ForceDirection);
    }
}


void AEOSActionGameState::DeleteChest_Implementation(int chestID, float timeToDisappear)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        if (AllChests.Num() <= 0)
            return;

        if (AllChests.IsValidIndex(chestID) == false)
            return;

        if (timeToDisappear <= 0)
        {
            for (AChestSpawnLocation* chestSpawn : AllChestsSpawnLocations)
            {
                if (chestSpawn->OwningChest == AllChests[chestID])
                {
                    chestSpawn->OwningChest = nullptr;
                    AllChests[chestID]->Destroy();
                    AllChests.RemoveAt(chestID);
                    return;
                }
            }
        }
        else
        {
            timeToDisappear -= GetWorld()->GetDeltaSeconds();
            FTimerHandle deleteTimerHandle;
            deleteTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::DeleteChest, chestID, timeToDisappear));
        }
    }
}

void AEOSActionGameState::Server_Ping_Implementation(FVector hitPoint, AActor* hitActor)
{
    if (hitActor == nullptr)
        return;

    //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit Chest: %s"), *hitActor->GetName()));

    FText iconText = FText::FromString("");
    float spawnPosAdjustment = 0.0f;
    FVector spawnPos = hitPoint;
    UTexture* iconImage = nullptr;

    FActorSpawnParameters SpawnParams;
    AItemChest* hitChest = Cast<AItemChest>(hitActor);
    if (hitChest)
    {
        if (hitChest->HasPing())
            return;

        spawnPos = hitChest->GetActorLocation();
        spawnPosAdjustment = 50.0f;
        iconImage = ChestIcon;
        iconText = FText::Format(FText::FromString("Cost: {0}"), FText::AsNumber(hitChest->ScrapPrice));
    }

    AItemPickup* hitItem = Cast<AItemPickup>(hitActor);
    if (hitItem)
    {
        if (hitItem->HasPing())
            return;

        spawnPos = hitItem->GetActorLocation();
        spawnPosAdjustment = 60.0f;
        iconImage = hitItem->ItemAsset->ItemIcon;
        iconText = FText::FromName(hitItem->ItemAsset->ItemName);
    }

    ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(hitActor);
    if (hitCharacter)
    {
        if (hitCharacter->HasPing())
            return;

        spawnPos = hitCharacter->GetActorLocation();
        spawnPosAdjustment = 60.0f;
        iconImage = hitCharacter->CharacterPingIcon;
        //iconImage = hitItem->ItemAsset->ItemIcon;
        //iconText = FText::FromName(hitItem->ItemAsset->ItemName);
    }

    AEscapeToWin* escapeToWin = Cast<AEscapeToWin>(hitActor);
    if (escapeToWin)
    {
        spawnPos = escapeToWin->GetActorLocation();
        spawnPosAdjustment = 70.0f;
        iconImage = GateIcon;
        //iconImage = hitItem->ItemAsset->ItemIcon;
        //iconText = FText::FromName(hitItem->ItemAsset->ItemName);
    }

    spawnPos.Z += spawnPosAdjustment;
    APingActor* newPing = GetWorld()->SpawnActor<APingActor>(PingActorClass, spawnPos, FRotator::ZeroRotator, SpawnParams);

    if(hitItem)
        hitItem->SetPairedPing(newPing);

    if (hitChest)
        hitChest->SetPairedPing(newPing);

    if (hitCharacter)
    {
        newPing->AttachToComponent(hitCharacter->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        hitCharacter->SetPairedPing(newPing);
    }

    newPing->SetOwner(this);
    newPing->SetIcons(iconImage, iconText);
    return;
}

bool AEOSActionGameState::Server_Ping_Validate(FVector hitPoint, AActor* hitActor)
{
    return true;
}

void AEOSActionGameState::Multicast_Ping_Implementation(FVector hitPoint, AActor* hitActor)
{
}

TArray<URItemDataAsset*> AEOSActionGameState::GetItemPoolOfRarity(TArray<URItemDataAsset*> itemPool, EItemRarity itemRarity)
{
    TArray<URItemDataAsset*> newItemPool;
    for(URItemDataAsset* item : itemPool)
    {
        if (item->ItemRarity == itemRarity)
        {
            newItemPool.Add(item);
        }
    }

    if (newItemPool.Num() <= 0)
        return itemPool;

    return newItemPool;
}


bool AEOSActionGameState::GetKeyCard()
{
    if (bGottenKeyCard)
        return false;

    float percentage = (OpenedChests) * 10.0f;
    UE_LOG(LogTemp, Error, TEXT("percentage %f"), percentage);

    if (FMath::RandRange(0, 100) <= percentage)
    {
        bGottenKeyCard = true;
        return true;
    }

    return false;
}


void AEOSActionGameState::Multicast_ShootTexUltimate_Implementation(UNiagaraSystem* SystemToSpawn, AActor* characterAttached, FVector SpawnLocation, FVector Direction, FVector endPos)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;

    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

    //UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        SpawnSystemAttached->SetVectorParameter(FName("BeamEnd"), endPos);
    }
}

void AEOSActionGameState::Multicast_RobotGiblets_Implementation(FVector SpawnLocation, FVector Direction, int amount)
{
    if (RobotGiblets == nullptr)
        return;

    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RobotGiblets, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);
    if (SpawnSystemAttached)
    {
        SpawnSystemAttached->SetVectorParameter(FName("Direction"), Direction);
        SpawnSystemAttached->SetIntParameter(FName("Amount"), amount);
    }
}

void AEOSActionGameState::Multicast_RequestPlayAudio_Implementation(ARCharacterBase* characterToAttach, USoundBase* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings)
{
    if (AttenuationSettings == nullptr && VoiceAttenuationSettings != nullptr)
        AttenuationSettings = VoiceAttenuationSettings;

    if (characterToAttach != nullptr)
    {
        UGameplayStatics::SpawnSoundAttached(Sound, characterToAttach->GetMesh(), NAME_None, Location, EAttachLocation::KeepRelativeOffset, true, 1, 1, 0, VoiceAttenuationSettings);
    }
    else
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings);
    }
}


void AEOSActionGameState::Multicast_RequestPlayAudioComponentRolling_Implementation(USoundBase* Sound, ARCharacterBase* characterToAttach, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings)
{
    if (AttenuationSettings == nullptr && VoiceAttenuationSettings != nullptr)
        AttenuationSettings = VoiceAttenuationSettings;

    //Multicast_StopComponentRolling(characterToAttach);

    UAudioComponent* NewComponent = UGameplayStatics::SpawnSoundAttached(Sound, characterToAttach->GetMesh(), NAME_None, Location, EAttachLocation::KeepRelativeOffset, true, 1, 1, 0, VoiceAttenuationSettings);
    if (NewComponent)
    {
        RollingAudioComponentsByCharacter.Add(characterToAttach, NewComponent);
    }
}

void AEOSActionGameState::Multicast_StopComponentRolling_Implementation(ARCharacterBase* characterToAttach)
{
    if (UAudioComponent** CompPtr = RollingAudioComponentsByCharacter.Find(characterToAttach))
    {
        if (UAudioComponent* Comp = *CompPtr)
        {
            Comp->Stop();
        }
        RollingAudioComponentsByCharacter.Remove(characterToAttach);
    }
}


void AEOSActionGameState::Server_RequestChangeItem_Implementation(AItemPickup* ChoosingItem, URItemDataAsset* chosenItem)
{
    for (AItemPickup* item : AllItems) // send back a failure so that they don't softlock
    {
        if (item == ChoosingItem)
        {
            item->SetupItem(chosenItem, FVector(0,0,0));
        }
    }
}

bool AEOSActionGameState::Server_RequestChangeItem_Validate(AItemPickup* ChoosingItem, URItemDataAsset* chosenItem)
{
    return true;
}


void AEOSActionGameState::LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad)
{

    if (!levelToLoad.IsValid())
    {
        levelToLoad.LoadSynchronous();
    }

    if (levelToLoad.IsValid())
    {
        for (APlayerState* playerState : PlayerArray)
        {
            AEOSPlayerState* eosPlayerState = Cast<AEOSPlayerState>(playerState);
            if (eosPlayerState)
            {
                eosPlayerState->Client_Load();
            }
        }

        const FName levelName = FName(*FPackageName::ObjectPathToPackageName(levelToLoad.ToString()));
        GetWorld()->ServerTravel(levelName.ToString() + "?listen");
    }
}

void AEOSActionGameState::Server_RequestSpawnVFXOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    Multicast_RequestSpawnVFXOnCharacter(SystemToSpawn, characterAttached, SpawnLocation, Direction, otherValue);
}

bool AEOSActionGameState::Server_RequestSpawnVFXOnCharacter_Validate(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}

void AEOSActionGameState::Server_RequestSpawnVFX_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{
    
}

bool AEOSActionGameState::Server_RequestSpawnVFX_Validate(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}

void AEOSActionGameState::Multicast_PlayerFullyDies_Implementation(ARPlayerBase* deadPlayer)
{
    if (deadPlayer == nullptr)
        return;

    deadPlayer->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetFullyDeadTag());

    deadPlayer->ItemPickupCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    deadPlayer->GroundCheckComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    deadPlayer->ItemPickupCollider->SetGenerateOverlapEvents(false);
    deadPlayer->GroundCheckComp->SetGenerateOverlapEvents(false);

    deadPlayer->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    deadPlayer->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    deadPlayer->GetMesh()->SetVisibility(false, true); // false = not visible, true = propagate to children
    deadPlayer->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    deadPlayer->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    deadPlayer->GetMesh()->SetGenerateOverlapEvents(false);

    deadPlayer->GetCapsuleComponent()->SetSimulatePhysics(false);
    deadPlayer->GetCapsuleComponent()->SetEnableGravity(false);
    deadPlayer->GetMesh()->SetSimulatePhysics(false);
    deadPlayer->GetMesh()->SetEnableGravity(false);

    deadPlayer->SetPlayerReviveState(false);

    if (deadPlayer->ReviveUI)
    {
        deadPlayer->ReviveUI->SetVisibility(ESlateVisibility::Hidden);
    }

    if (deadPlayer->HealthBar)
    {
        deadPlayer->HealthBar->SetVisibility(ESlateVisibility::Hidden);
    }


    AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
    if (GameState == nullptr)
        return;

    for (APlayerState* playerState : GameState->PlayerArray)
    {
        ARPlayerBase* character = Cast<ARPlayerBase>(playerState->GetPawn());
        if (character == nullptr)
            continue;

        if (character == deadPlayer)
        {
            AEOSPlayerState* RabiesPlayerState = Cast<AEOSPlayerState>(playerState);
            if (RabiesPlayerState)
            {
                RabiesPlayerState->SetFullyDead(true, deadPlayer);
            }
            continue;
        }
        else if(character->nearbyFaintedActors.Contains(deadPlayer))
        {
            character->nearbyFaintedActors.Remove(deadPlayer);
        }

        if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
            continue;

        deadPlayer->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
        FVector spawnPos = FVector(0, 0, 50);
        deadPlayer->SetActorRelativeLocation(spawnPos /*FTransform::Identity*/);

    }
}

void AEOSActionGameState::Multicast_StickPlayerOnDot_Implementation(ARPlayerBase* stickingPlayer, ARPlayerBase* dot, bool bStick)
{
    if (stickingPlayer == nullptr || dot == nullptr)
        return;

    AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
    if (GameState == nullptr)
        return;


    for (APlayerState* playerState : GameState->PlayerArray)
    {
        ARPlayerBase* character = Cast<ARPlayerBase>(playerState->GetPawn());
        if (character == nullptr)
            continue;

        if (character == stickingPlayer)
        {
            AEOSPlayerState* RabiesPlayerState = Cast<AEOSPlayerState>(playerState);
            if (RabiesPlayerState)
            {
                RabiesPlayerState->SetStickDot(bStick, dot);
            }
        }

        ECollisionResponse response = (bStick) ? ECR_Ignore : ECR_Block;
        //stickingPlayer->SetReplicateMovement(!bStick);
        stickingPlayer->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, response);

        FVector FootLocation = dot->GetMesh()->GetSocketLocation("grabPoint");

        stickingPlayer->NetMulticast_SetDotHijack((bStick) ? dot : nullptr);
        
        FHitResult HitResult;
        bool bSuccess = stickingPlayer->SetActorLocation(FootLocation, true, &HitResult);
        if (bSuccess && stickingPlayer->bWindingUp == false)
        {
            stickingPlayer->GetCharacterMovement()->StopMovementImmediately();
            stickingPlayer->SetActorLocation(FootLocation /*FTransform::Identity*/);
        }
    }
}

void AEOSActionGameState::Multicast_CenterOnDot_Implementation(ARPlayerBase* stickingPlayer, ARPlayerBase* dot)
{
    if (stickingPlayer == nullptr || dot == nullptr)
        return;

    AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
    if (GameState == nullptr)
        return;
    
    FVector FootLocation = dot->GetMesh()->GetSocketLocation("grabPoint");

    FHitResult HitResult;
    bool bSuccess = stickingPlayer->SetActorLocation(FootLocation, true, &HitResult);
    if (bSuccess && stickingPlayer->bWindingUp == false)
    {
       stickingPlayer->GetCharacterMovement()->StopMovementImmediately();
       stickingPlayer->SetActorLocation(FootLocation);
    }
}


void AEOSActionGameState::Multicast_AdjustIceOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!characterAttached) return;

    if (otherValue <= 0)
    {
        if (characterAttached->DynamicMaterialInstance)
        {
            characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        if (characterAttached->DynamicOtherMaterialInstance)
        {
            characterAttached->DynamicOtherMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.868313f, 1.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), 0.2f);
    }
    if (characterAttached->DynamicOtherMaterialInstance)
    {
        characterAttached->DynamicOtherMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.868313f, 1.0f));
        characterAttached->DynamicOtherMaterialInstance->SetScalarParameterValue(FName("Intensity"), 0.2f);
    }
}

void AEOSActionGameState::Server_AdjustIceOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    Multicast_AdjustIceOnCharacter(SystemToSpawn, characterAttached, SpawnLocation, Direction, otherValue);
}

bool AEOSActionGameState::Server_AdjustIceOnCharacter_Validate(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    return true;
}


void AEOSActionGameState::Multicast_AdjustFireOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;

    //UE_LOG(LogTemp, Error, TEXT("adjusting fire"));

    float max = (otherValue * 1.5f) * characterAttached->WeakpointSize;
    float min = (otherValue) * characterAttached->WeakpointSize;

    max = FMath::Clamp(max, 0, 170);
    min = FMath::Clamp(min, 0, 170);

    if (characterAttached->CurrentFire == nullptr)
    {
        characterAttached->CurrentFire = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);
        if (characterAttached->CurrentFire)
        {
            characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMax"), max);
            characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMin"), min);
        }
        return;
    }


    if (otherValue <= 0 && characterAttached->CurrentFire)
    {
        characterAttached->CurrentFire->Deactivate();
        characterAttached->CurrentFire->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        characterAttached->CurrentFire->DestroyComponent();
        characterAttached->CurrentFire = nullptr;
        if (characterAttached->DynamicMaterialInstance)
        {
            characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        if (characterAttached->DynamicOtherMaterialInstance)
        {
            characterAttached->DynamicOtherMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        float normalized = min / 170.0f;
        float fireIntensity = FMath::Lerp(2.0f, 0.1f, normalized);

        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(1.0f, 0.165817f, 0.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), fireIntensity);
    }

    if (characterAttached->DynamicOtherMaterialInstance)
    {
        float normalized = min / 170.0f;
        float fireIntensity = FMath::Lerp(2.0f, 0.1f, normalized);

        characterAttached->DynamicOtherMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(1.0f, 0.165817f, 0.0f));
        characterAttached->DynamicOtherMaterialInstance->SetScalarParameterValue(FName("Intensity"), fireIntensity);
    }

    characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMax"), max);
    characterAttached->CurrentFire->SetFloatParameter(FName("BaseSizeMin"), min);
}

void AEOSActionGameState::Multicast_RequestSpawnVFXOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;
    
    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        //NiagaraComp->SetVectorParameter(FName("Alignment"), Direction);
    }
}

void AEOSActionGameState::Multicast_RequestSpawnVFX_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float otherValue)
{
    if (!SystemToSpawn) return;

    UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

    if (NiagaraComp && otherValue == 102)
    {
        NiagaraComp->SetVectorParameter(FName("Direction"), Direction);
    }
}

void AEOSActionGameState::Multicast_SpawnHealingPulse_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float healingRadius)
{
    if (!HealingPulse) return;
    if (!characterAttached) return;

    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(HealingPulse, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        SpawnSystemAttached->SetFloatParameter(FName("circleSize"), healingRadius * 2.25f);
        SpawnSystemAttached->SetFloatParameter(FName("heartSize"), healingRadius * 1.0f);
        SpawnSystemAttached->SetFloatParameter(FName("confettiSize"), healingRadius * 0.75f);
    }
}

void AEOSActionGameState::Multicast_SpawnRadio_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float size)
{
    if (!SystemToSpawn) return;
    if (!characterAttached) return;

    UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAttached(SystemToSpawn, characterAttached->GetMesh(), NAME_None, SpawnLocation, FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, true, true, ENCPoolMethod::None, true);

    if (SpawnSystemAttached)
    {
        size = FMath::Clamp(size, 0.0f, 150.0f);

        SpawnSystemAttached->SetFloatParameter(FName("amount"), size * 0.25f);
        SpawnSystemAttached->SetFloatParameter(FName("lifetime"), size * 0.1f);
        SpawnSystemAttached->SetFloatParameter(FName("finalSize"), size);
    }
}


void AEOSActionGameState::Server_RequestSpawnDotLaserMarks_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    Multicast_SpawnDotLaserMarks(SystemToSpawn, SpawnLocation, Direction, ScorchSize);
}

bool AEOSActionGameState::Server_RequestSpawnDotLaserMarks_Validate(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    return true;
}

void AEOSActionGameState::Multicast_SpawnDotLaserMarks_Implementation(UNiagaraSystem* SystemToSpawn, FVector SpawnLocation, FVector Direction, float ScorchSize)
{
    if (!SystemToSpawn) return;

    UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SystemToSpawn, SpawnLocation, FRotator::ZeroRotator, FVector(1.0f), true, true);

    if (NiagaraComp)
    {
        NiagaraComp->SetVectorParameter(FName("Alignment"), Direction);
        NiagaraComp->SetFloatParameter(FName("ScorchSize"), ScorchSize);
    }
}


void AEOSActionGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChests, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllEnemies, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllItems, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, bInstantKeyCard, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, OpenedChests, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, AllChestsSpawnLocations, COND_None);
    DOREPLIFETIME_CONDITION(AEOSActionGameState, GameSettings, COND_None);
}