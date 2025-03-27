// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemChest.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Framework/EOSActionGameState.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Abilities/GameplayAbility.h"

#include "Net/UnrealNetwork.h"

#include "ScalableFloat.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"

#include "Widgets/ChestInteractUI.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItemChest::AItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicates(true);

	ChestBottomMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Bottom Mesh");
	ChestBottomMesh->SetupAttachment(GetRootComponent());
	ChestBottomMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	RootComponent = ChestBottomMesh;
	
	ChestTopMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Top Mesh");
	ChestTopMesh->SetupAttachment(GetRootComponent());
	ChestTopMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemChest::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItemChest::OnOverlapEnd);

	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	InteractWidgetComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItemChest::BeginPlay()
{
	Super::BeginPlay();

	SetUpUI(true);
}

// Called every frame
void AItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);
	if (!player || bWasOpened)
	{
		return;
	}

	bWithinInteraction = true;
	InteractWidget->SetVisibility(ESlateVisibility::Visible);

	player->PlayerInteraction.AddUObject(this, &AItemChest::Interact);

}

void AItemChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	player = Cast<ARPlayerBase>(OtherActor);
	if (!player)
	{
		return;
	}

	bWithinInteraction = false;
	InteractWidget->SetVisibility(ESlateVisibility::Hidden);

	player->PlayerInteraction.Clear();
}

void AItemChest::SetUpUI(bool SetInteraction)
{
	InteractWidget = Cast<UChestInteractUI>(InteractWidgetComp->GetUserWidgetObject());
	InteractWidget->SetCostText(ScrapPrice);
	InteractWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AItemChest::Interact()
{
	if (bWasOpened)
		return;

	player->SetInteractionChest(this);
}

void AItemChest::Server_OpenChest_Implementation()
{
	if (HasAuthority())
	{
		if (ScrapPrice > player->GetCurrentScrap())
		{
			UE_LOG(LogTemp, Error, TEXT("Not Enough Scrap"));
			return;
		}

		if (bWasOpened)
		{
			UE_LOG(LogTemp, Error, TEXT("Already Opened"));
			return;
		}

		UAbilitySystemComponent* ASC = player->GetAbilitySystemComponent();
		if (ASC && ScrapPriceEffect)
		{
			FGameplayEffectContextHandle effectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle effectSpecHandle = ASC->MakeOutgoingSpec(ScrapPriceEffect, 1.0f, effectContext);
			ASC->ApplyGameplayEffectSpecToSelf(*effectSpecHandle.Data.Get());
			AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
			if (gameState == GetOwner())
			{
				gameState->SelectChest(this);
			}
		}
	}
}


void AItemChest::UpdateChestOpened_Implementation()
{
	InteractWidget->SetVisibility(ESlateVisibility::Hidden);
	bWasOpened = true;
}

void AItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AItemChest, bWasOpened, COND_None);
}