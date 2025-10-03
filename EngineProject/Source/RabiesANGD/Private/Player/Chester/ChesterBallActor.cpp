// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/ChesterBallActor.h"

// Sets default values
AChesterBallActor::AChesterBallActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	Ball = CreateDefaultSubobject<UStaticMeshComponent>("Ball");
	RootComponent = Ball;
	Ball->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AChesterBallActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChesterBallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChesterBallActor::SetActiveState(bool state)
{
	SetActorHiddenInGame(state);
}

