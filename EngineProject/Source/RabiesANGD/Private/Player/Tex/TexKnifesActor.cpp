// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/TexKnifesActor.h"

// Sets default values
ATexKnifesActor::ATexKnifesActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	knife1 = CreateDefaultSubobject<UStaticMeshComponent>("Knife1");
	RootComponent = knife1;
	knife1->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	knife2 = CreateDefaultSubobject<UStaticMeshComponent>("Knife2");
	knife2->SetupAttachment(GetRootComponent());
	knife2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	knife3 = CreateDefaultSubobject<UStaticMeshComponent>("Knife3");
	knife3->SetupAttachment(GetRootComponent());
	knife3->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	knife4 = CreateDefaultSubobject<UStaticMeshComponent>("Knife4");
	knife4->SetupAttachment(GetRootComponent());
	knife4->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	knife5 = CreateDefaultSubobject<UStaticMeshComponent>("Knife5");
	knife5->SetupAttachment(GetRootComponent());
	knife5->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATexKnifesActor::BeginPlay()
{
	Super::BeginPlay();
	
	//->SetWorldScale3D(FVector(0.1f,0.1f,0.1f));
}

// Called every frame
void ATexKnifesActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATexKnifesActor::SetActiveState(bool state)
{
	SetActorHiddenInGame(state);
}

