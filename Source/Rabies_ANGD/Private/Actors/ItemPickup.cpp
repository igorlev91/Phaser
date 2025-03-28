// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemPickup.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Framework/RItemDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Abilities/GameplayAbility.h"

#include "Blueprint/UserWidget.h"

#include "Framework/EOSActionGameState.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("Item Mesh");
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	RootComponent = ItemMesh;

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AItemPickup::Server_PickupItem_Implementation()
{
	if (HasAuthority())
	{
		UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
		if (ASC && ItemAsset)
		{
			FGameplayEffectContextHandle effectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle effectSpecHandle = ASC->MakeOutgoingSpec(ItemAsset->ItemEffect, 1.0f, effectContext);
			ASC->ApplyGameplayEffectSpecToSelf(*effectSpecHandle.Data.Get());
			AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
			if (gameState == GetOwner())
			{
				gameState->SelectItem(this, Player);
			}
		}
	}
}

void AItemPickup::PlayerPickupRequest_Implementation(ARPlayerBase* player)
{
	Player = player;
	Server_PickupItem();
}

void AItemPickup::UpdateItemPickedup_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Player picked up item"));
	
	Player->PickupAudio = ItemAsset->ItemAudio;

	if (Player->PickupAudio)
	{
		Player->PlayPickupAudio();
	}

	//Player->AddItem(ItemAsset);
	Destroy();
}

// Called every frame
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickup::SetupItem_Implementation(URItemDataAsset* newItemAsset)
{
	ItemAsset = newItemAsset;

	if (newItemAsset->ItemMesh == nullptr)
	{
		ItemMesh->SetStaticMesh(NullMesh);
	}
	else
	{
		ItemMesh->SetStaticMesh(newItemAsset->ItemMesh);
	}

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}