// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CagedCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACagedCharacter::ACagedCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create and initialize the Static Mesh Component
    CagedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    CagedMesh->SetupAttachment(GetRootComponent());

    RootComponent = CagedMesh;

    CagedCharacter = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    CagedCharacter->SetupAttachment(CagedMesh);
}

// Called when the game starts or when spawned
void ACagedCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACagedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACagedCharacter::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
}

void ACagedCharacter::TickPosition(FVector Goal, FVector Sideline, bool goToSideline)
{
	FVector CurrentOffset = GetActorLocation();
	if (FVector::Dist(CurrentOffset, Goal) < 5)
	{
		SetActorLocation((goToSideline) ? Sideline : Goal);
		return;
	}

	//FVector NewOffset = FMath::Lerp(CurrentOffset, Goal, GetWorld()->GetDeltaSeconds() * 2.0f);
	FVector Direction = (Goal - CurrentOffset).GetSafeNormal();
	FVector NewOffset = CurrentOffset + Direction * 12.0f;
	SetActorLocation(NewOffset);
	MovementTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ACagedCharacter::TickPosition, Goal, Sideline, goToSideline));
}
