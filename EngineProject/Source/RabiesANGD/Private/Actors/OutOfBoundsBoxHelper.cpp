// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/OutOfBoundsBoxHelper.h"
#include "Components/BoxComponent.h"

#include "Player/RPlayerBase.h"
#include "Enemy/REnemyBase.h"

// Sets default values
AOutOfBoundsBoxHelper::AOutOfBoundsBoxHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false; // doesn't need to exist on clients

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Box->OnComponentBeginOverlap.AddDynamic(this, &AOutOfBoundsBoxHelper::OnOverlapBegin);

	TeleportLocation = FVector(0.f, 0.f, 500.f); // Default fallback location
}

// Called when the game starts or when spawned
void AOutOfBoundsBoxHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOutOfBoundsBoxHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOutOfBoundsBoxHelper::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor)
		return;

	ARPlayerBase* Player = Cast<ARPlayerBase>(OtherActor); // Replace with your actual player class
	if (Player)
	{
		Player->SetActorLocation(TeleportLocation);
		UE_LOG(LogTemp, Warning, TEXT("Teleported %s back to stage."), *Player->GetName());
		return;
	}

	AREnemyBase* enemy = Cast<AREnemyBase>(OtherActor); // Replace with your actual player class
	if (enemy)
	{
		enemy->SetActorLocation(TeleportLocation);
		UE_LOG(LogTemp, Warning, TEXT("Teleported %s back to stage."), *enemy->GetName());
	}
}