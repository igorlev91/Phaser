// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Actors/EscapeToWin.h"
#include "Actors/ItemPickup.h"
#include "Character/RCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "Math/UnrealMathUtility.h"
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
#include "NiagaraComponent.h"

void AEOSActionGameState::BeginPlay()
{
    Super::BeginPlay();
	// this will be on server side

    if (HasAuthority() == false)
        return;

    TArray<AActor*> EscapeToWinObj;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEscapeToWin::StaticClass(), EscapeToWinObj);
    EscapeToWin = Cast<AEscapeToWin>(EscapeToWinObj[0]);
    if (EscapeToWin)
    {
        EscapeToWin->SetOwner(this);
    }

    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChestSpawnLocation::StaticClass(), spawnLocations);

    if (spawnLocations.Num() <= AmountOfChests) return;
    MaxChests = AmountOfChests;

    for (int i = 0; i < AmountOfChests; i++)
    {
        float randomSpawn = FMath::RandRange(0, spawnLocations.Num() - 1);
        AChestSpawnLocation* thisSpawn = Cast<AChestSpawnLocation>(spawnLocations[randomSpawn]);

        if (thisSpawn != nullptr)
        {
            float randomItemCost = FMath::RandRange(3, 10);
            SpawnChest(thisSpawn->GetActorLocation(), thisSpawn->bRareChest, randomItemCost);
            spawnLocations.RemoveAt(randomSpawn);
        }
    }

    TArray<AActor*> enemySpawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), enemySpawnLocations);

    WaveSpawnCap = FMathf::Clamp(WaveSpawnCap, 1, enemySpawnLocations.Num());

    WaveLevel = 0;
    WaveTime = enemyInitalSpawnRate;
    WaveHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEOSActionGameState::WaveSpawn, 0.0f));
}

void AEOSActionGameState::SpawnChest_Implementation(FVector SpawnLocation, bool bRareChest, float randomItemCost)
{
    if (HasAuthority() && ItemChestClass != nullptr && GetWorld() != nullptr) // Ensure we're on the server
    {
        FActorSpawnParameters SpawnParams;
        AItemChest* newChest = GetWorld()->SpawnActor<AItemChest>(ItemChestClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        newChest->SetOwner(this);
        if (bRareChest)
        {
            newChest->ScrapPrice = 80.0f;
        }
        else
        {
            newChest->ScrapPrice = randomItemCost * 5.0f;
        }
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
    LoadMapAndListen(SelectLevel);
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

    WaveTime += 2.0f;

    TArray<AActor*> spawnLocations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnLocation::StaticClass(), spawnLocations);

    int spawnsThisWave = 0;
    float power = powerLevel;

    while (power > 0 && spawnsThisWave < WaveSpawnCap)
    {
        if (AllEnemies.Num() >= enemyMax)
            return;

        UE_LOG(LogTemp, Warning, TEXT("Power is: %f"), power);

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

        FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AEOSActionGameState::SpawnEnemy, enemyID, spawnLocations[randomSpawn]->GetActorLocation(), true);
        FTimerHandle newSpawnHandle;
        GetWorldTimerManager().SetTimer(newSpawnHandle, TimerDelegate, 0.1f, false);
        spawnsThisWave++;
        spawnLocations.RemoveAt(randomSpawn);
    }
}

void AEOSActionGameState::StartBossFight_Implementation(int enemyID)
{
    if (enemyID == 1)
        deadlockPos.X += 300;

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

void AEOSActionGameState::PickedUpItem_Implementation(int itemID, ARPlayerBase* targetingPlayer)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        if (AllItems[itemID]->ItemAsset == KeyCard)
        {
            for (APlayerState* playerState : PlayerArray)
            {
                if (playerState && playerState->GetPawn())
                {
                    ARPlayerBase* player = Cast<ARPlayerBase>(playerState->GetPawn());
                    if (player)
                    {
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
            targetingPlayer->AddNewItem(AllItems[itemID]->ItemAsset);
        }
        AllItems[itemID]->UpdateItemPickedup();
        AllItems.RemoveAt(itemID);
    }
}

void AEOSActionGameState::OpenedChest_Implementation(int chestID, int spawnCount)
{
    if (HasAuthority()) // Ensure we're on the server
    {
        AllChests[chestID]->UpdateChestOpened();

        for (int i = 0; i < spawnCount; i++)
        {
            URItemDataAsset* newData = ItemLibrary[0];
            TArray<URItemDataAsset*> ChosenLibrary = (ItemSelection.IsEmpty() == false) ? ItemSelection : ItemLibrary;

            bool bRareItem = false;

            float randomItemRariety = FMath::RandRange(0, 75);
            if (AllChests[chestID]->ScrapPrice > randomItemRariety)
            {
                float reRollRandomItemRariety = FMath::RandRange(0, 100);
                if (AllChests[chestID]->ScrapPrice > reRollRandomItemRariety || AllChests[chestID]->ScrapPrice >= 80)
                {
                    bRareItem = true;
                    ChosenLibrary = GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Rare);
                }
                else
                {
                    ChosenLibrary = GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Uncommon);
                }
            }
            else
            {
                ChosenLibrary = GetItemPoolOfRarity(ChosenLibrary, EItemRarity::Common);
            }

            int32 randomIndex = FMath::RandRange(0, ChosenLibrary.Num() - 1);
            newData = ChosenLibrary[randomIndex];

            if (GetKeyCard() && bRareItem == false)
                newData = KeyCard;

            FActorSpawnParameters SpawnParams;
            FVector spawnAdjustment = AllChests[chestID]->GetActorLocation();
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
        AllChests.RemoveAt(chestID);
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

    AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitActor);
    if (hitEnemy)
    {
        spawnPos = hitEnemy->GetActorLocation();
        spawnPosAdjustment = 60.0f;
        iconImage = hitEnemy->EnemyIcon;
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

    if (hitEnemy)
        newPing->AttachToComponent(hitEnemy->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

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
    if (MaxChests <= 0.0f)
        return false;

    if (bGottenKeyCard)
        return false;

    float percentage = ((float)AllChests.Num() / (float)MaxChests) * 90.0f;

    if (FMath::RandRange(0, 100) >= percentage)
    {
        bGottenKeyCard = true;
        return true;
    }

    return false;
}


void AEOSActionGameState::Multicast_ShootTexUltimate_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, FVector endPos)
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

void AEOSActionGameState::Multicast_RequestPlayAudio_Implementation(USoundBase* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings)
{
    UGameplayStatics::PlaySoundAtLocation(this, Sound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings);
}

void AEOSActionGameState::LoadMapAndListen(TSoftObjectPtr<UWorld> levelToLoad)
{
    if (!levelToLoad.IsValid())
    {
        levelToLoad.LoadSynchronous();
    }

    if (levelToLoad.IsValid())
    {
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


void AEOSActionGameState::Multicast_AdjustIceOnCharacter_Implementation(UNiagaraSystem* SystemToSpawn, ARCharacterBase* characterAttached, FVector SpawnLocation, FVector Direction, float otherValue)
{
    //if (!SystemToSpawn) return;
    if (!characterAttached) return;

    if (otherValue <= 0)
    {
        if (characterAttached->DynamicMaterialInstance)
        {
            characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.0f, 0.0f));
        }
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(0.0f, 0.868313f, 1.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), 0.2f);
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
        return;
    }

    if (characterAttached->DynamicMaterialInstance)
    {
        float normalized = min / 170.0f;
        float fireIntensity = FMath::Lerp(2.0f, 0.1f, normalized);

        characterAttached->DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), FVector(1.0f, 0.165817f, 0.0f));
        characterAttached->DynamicMaterialInstance->SetScalarParameterValue(FName("Intensity"), fireIntensity);
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
}