// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Dot/RTargetActor_DotUltimate.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/RPlayerBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Engine/World.h"
#include "Components/DecalComponent.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"

ARTargetActor_DotUltimate::ARTargetActor_DotUltimate()
{
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    ShouldProduceTargetDataOnServer = true;

    // Create Cylinder Mesh
    CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
    RootComponent = CylinderMesh;

    // Assign the default cylinder mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderAsset.Succeeded())
    {
        CylinderMesh->SetStaticMesh(CylinderAsset.Object);
    }

	SetRootComponent(CylinderMesh);

    CylinderMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CylinderMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    CylinderMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    CylinderMesh->SetIsReplicated(true);
}

void ARTargetActor_DotUltimate::Server_SetCylinderSize_Implementation(FVector scale)
{
    CylinderMesh->SetRelativeScale3D(scale);
}