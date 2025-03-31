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

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetSimulatePhysics(true);
	RootComponent = ItemMesh;

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	SphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

	SphereStyle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Mesh"));
	SphereStyle->SetupAttachment(ItemMesh);
	SphereStyle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereStyle->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

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

	if (bRotate)
	{
		FRotator CurrentRotation = ItemMesh->GetComponentRotation();
		FRotator NewRotation = CurrentRotation + FRotator(0.f, 30 * DeltaTime, 0.f); // Rotate around Y axis (yaw)

		ItemMesh->SetWorldRotation(NewRotation);
	}
}

void AItemPickup::SetupItem_Implementation(URItemDataAsset* newItemAsset)
{
	ItemAsset = newItemAsset;
	if (newItemAsset->OutlineInstance != nullptr)
	{
		ItemMesh->SetOverlayMaterial(newItemAsset->OutlineInstance);
	}
	if (newItemAsset->OrbInstance != nullptr)
	{
		SphereStyle->SetMaterial(0, newItemAsset->OrbInstance);
	}

	if (newItemAsset->ItemMesh == nullptr)
	{
		ItemMesh->SetStaticMesh(NullMesh);
	}
	else
	{
		ItemMesh->SetStaticMesh(newItemAsset->ItemMesh);
	}

	float x = FMath::RandRange(-100, 100);
	float y = FMath::RandRange(-100, 100);
	FVector ForceDirection = FVector(x, y, 600.f); // Adjust the Z value to set how much it pops up
	ItemMesh->AddImpulse(ForceDirection, NAME_None, true);
	GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, this, &AItemPickup::StartRotatingItem, 1.2f, false);
}

void AItemPickup::StartRotatingItem_Implementation()
{
	ItemMesh->SetSimulatePhysics(false);
	bRotate = true;
}