// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AbilityHitBox.h"
#include "Components/StaticMeshComponent.h"
#include "Player/RCharacterSelectController.h"
#include "Engine/Engine.h"
#include "Components/SpotLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Actors/Clipboard.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
AAbilityHitBox::AAbilityHitBox()
{
	PrimaryActorTick.bCanEverTick = true;

	MyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MyMeshComponent;

	MyMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MyMeshComponent->SetGenerateOverlapEvents(true);

	this->SetActorEnableCollision(true);
	this->SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void AAbilityHitBox::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ClipboardSeach;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClipboard::StaticClass(), ClipboardSeach);
	OwnerClipboard = Cast<AClipboard>(ClipboardSeach[0]);
}

// Called every frame
void AAbilityHitBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAbilityHitBox::SetIndex(int newIndex)
{
	index = newIndex;
}



void AAbilityHitBox::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	if (OwnerClipboard)
	{
		//if (GEngine) 
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit Chest: %f"), (float)index));

		OwnerClipboard->OpenMovie(index);
	}
}

void AAbilityHitBox::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unhovered"));

	if (OwnerClipboard)
	{
		OwnerClipboard->CloseMovie();
	}
}

