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
#include "Actors/PingActor.h"

#include "Blueprint/UserWidget.h"

#include "Framework/EOSActionGameState.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/StaticMeshActor.h"


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
	SphereStyle->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereStyle->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	SphereStyle->SetSimulatePhysics(false);
	SphereStyle->SetEnableGravity(false);

}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, this, &AItemPickup::StartLookingForGround, 0.1f, false);
}

void AItemPickup::StartLookingForGround()
{
	SphereStyle->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SphereStyle->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	SphereStyle->SetGenerateOverlapEvents(true);

	SphereStyle->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnItemHit);
}

void AItemPickup::Server_PickupItem_Implementation()
{
	if (HasAuthority())
	{
		if (ItemAsset)
		{
			if (ItemAsset->ItemTooltip == "CommonToyBox" || ItemAsset->ItemTooltip == "UncommonToyBox" || ItemAsset->ItemTooltip == "RareToyBox")
			{
				return;
			}
		}
		else
		{
			return;
		}

		UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
		if (ASC)
		{
			FGameplayEffectContextHandle effectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle effectSpecHandle = ASC->MakeOutgoingSpec(ItemAsset->ItemEffect, 1.0f, effectContext);
			ASC->ApplyGameplayEffectSpecToSelf(*effectSpecHandle.Data.Get());
			if (ItemAsset->ItemName == "Ammo Box")
			{
				Player->attackSpeedBeforeSpecial -= 0.2f;
			}
			else if (ItemAsset->ItemName == "Laser Sight")
			{
				Player->attackSpeedBeforeSpecial -= 0.1f;
			}
			else if (ItemAsset->ItemName == "Speed Pills")
			{
				Player->attackSpeedBeforeSpecial -= 0.05f;
			}

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
	if (MyPing != nullptr)
		MyPing->TimedDestroy();

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

void AItemPickup::SetPairedPing(APingActor* myPing)
{
	if (MyPing == nullptr)
	{
		MyPing = myPing;
	}
}

bool AItemPickup::HasPing()
{
	return (MyPing != nullptr);
}

void AItemPickup::SetupItem_Implementation(URItemDataAsset* newItemAsset, FVector randomDirection)
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

	ItemMesh->AddImpulse(randomDirection, NAME_None, true);
	//GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, this, &AItemPickup::StartRotatingItem, 1.2f, false);
}

void AItemPickup::StartRotatingItem_Implementation()
{
	ItemMesh->SetSimulatePhysics(false);
	SphereStyle->SetSimulatePhysics(false);
	SphereStyle->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	SphereStyle->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	bRotate = true;
}

void AItemPickup::OnItemHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp)
	{
		// Stop physics simulation
		StartRotatingItem();
		//GetWorld()->GetTimerManager().ClearTimer(FallTimerHandle);
	}
}

void AItemPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AItemPickup, MyPing, COND_None);
}