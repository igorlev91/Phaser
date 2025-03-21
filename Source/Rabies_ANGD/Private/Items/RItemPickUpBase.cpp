// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RItemPickUpBase.h"
#include "Components/SphereComponent.h"
#include "Player/RPlayerBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARItemPickUpBase::ARItemPickUpBase()
{
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	RootComponent = SphereCollider;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Upgrade Mesh"));
	ItemMesh->SetupAttachment(SphereCollider);
	ItemMesh->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

}

// Called when the game starts or when spawned
void ARItemPickUpBase::BeginPlay()
{
	Super::BeginPlay();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ARItemPickUpBase::OnSphereOverlap);
	
}

void ARItemPickUpBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPlayerBase* Player = Cast<ARPlayerBase>(OtherActor);

	if (Player)
	{
		GiveUpgrade();
		Destroy();
	}
}

void ARItemPickUpBase::GiveUpgrade()
{
	UE_LOG(LogTemp, Error, TEXT("Give Player an Upgrade"));
}

