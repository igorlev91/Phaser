// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RPhysicalButtonBase.h"
#include "Components/StaticMeshComponent.h"
#include "Player/RCharacterSelectController.h"
#include "Engine/Engine.h"
#include "Components/SpotLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"

// Sets default values
ARPhysicalButtonBase::ARPhysicalButtonBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MyMeshComponent;

	MyMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SpotLightComponent->SetupAttachment(MyMeshComponent); // Attach to SkeletalMeshComponent

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComponent->SetupAttachment(MyMeshComponent); // Attach to SkeletalMeshComponent
	TextRenderComponent->SetHorizontalAlignment(EHTA_Center);
	TextRenderComponent->SetWorldSize(50.f);

	this->SetActorEnableCollision(true);
	this->SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void ARPhysicalButtonBase::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterSelectController = Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void ARPhysicalButtonBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPhysicalButtonBase::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	// Your custom logic when the actor is clicked
	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Button Clicked!"));
	}
}

void ARPhysicalButtonBase::SetLight(bool state)
{
	SpotLightComponent->SetVisibility(state);
}

void ARPhysicalButtonBase::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	OnActorClicked(this, ButtonPressed);
}

