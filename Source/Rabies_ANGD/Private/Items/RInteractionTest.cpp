// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RInteractionTest.h"
#include "Components/SphereComponent.h"
#include "Player/RPlayerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Items/RItemPickUpBase.h"
#include "Framework/RGameMode.h"


// Sets default values
ARInteractionTest::ARInteractionTest()
{
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	RootComponent = SphereCollider;

	InteractMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interact Mesh"));
	InteractMesh->SetupAttachment(SphereCollider);
	InteractMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

// Called when the game starts or when spawned
void ARInteractionTest::BeginPlay()
{
	Super::BeginPlay();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ARInteractionTest::OnSphereOverlap);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &ARInteractionTest::OnSphereEndOverlap);
}

void ARInteractionTest::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPlayerBase* Player = Cast<ARPlayerBase>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(true);
		//SpawnItem();
	}
}

void ARInteractionTest::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPlayerBase* Player = Cast<ARPlayerBase>(OtherActor);

	if (Player)
	{
		Player->SetInteraction(false);
	}
}

void ARInteractionTest::SpawnItem()
{
	if (ItemSpawned != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetActorRotation();
			const FVector SpawnLocation = GetActorLocation() + FVector(100,0,0);

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			World->SpawnActor<ARItemPickUpBase>(ItemSpawned, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	//Spawn the new item
	Destroy();
}

