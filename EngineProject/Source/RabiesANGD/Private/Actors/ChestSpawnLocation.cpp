// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ChestSpawnLocation.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AChestSpawnLocation::AChestSpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultApparence = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	DefaultApparence->SetupAttachment(GetRootComponent());
	DefaultApparence->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = DefaultApparence;
}

void AChestSpawnLocation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AChestSpawnLocation, OwningChest, COND_None);
}

// Called when the game starts or when spawned
void AChestSpawnLocation::BeginPlay()
{
	Super::BeginPlay();

	DefaultApparence->SetVisibility(false);
}

// Called every frame
void AChestSpawnLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

