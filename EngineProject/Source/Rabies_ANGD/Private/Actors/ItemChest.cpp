// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemChest.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/PingUI.h"
#include "Actors/PingActor.h"
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

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AItemChest::AItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	//SetReplicates(true);

	ChestBottomMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Bottom Mesh");
	ChestBottomMesh->SetupAttachment(GetRootComponent());
	ChestBottomMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	RootComponent = ChestBottomMesh;
	
	ChestTopMesh = CreateDefaultSubobject<UStaticMeshComponent>("Chest Top Mesh");
	ChestTopMesh->SetupAttachment(GetRootComponent());
	ChestTopMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	InteractWidgetComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
}

void AItemChest::OpenChestAnim_Implementation()
{
}

// Called when the game starts or when spawned
void AItemChest::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FixTimer, this, &AItemChest::FixDisplay, 0.2f, false);
}

void AItemChest::FixDisplay()
{
	SetUpUI_Implementation(false);
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

	if (InteractWidget != nullptr)
	{
		InteractWidget->SetVisibility(ESlateVisibility::Visible);
	}

	player->SetInteractionChest(this);
	player->PlayerInteraction.AddUObject(this, &AItemChest::Interact);

}

void AItemChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	player = Cast<ARPlayerBase>(OtherActor);
	if (!player)
	{
		return;
	}

	if (SphereCollider->IsOverlappingActor(player))
	{
		// Still overlapping
		return;
	}

	bWithinInteraction = false;

	if (InteractWidget != nullptr)
	{
		InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	player->SetInteractionChest(nullptr);
	player->PlayerInteraction.Clear();
}

void AItemChest::SetUpUI_Implementation(bool SetInteraction)
{
	if (InteractWidgetComp == nullptr)
		return;

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemChest::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItemChest::OnOverlapEnd);

	//if (InteractWidget != nullptr)
		//return;

	InteractWidgetComp->SetWidgetClass(InteractUIClass);
	InteractWidget = CreateWidget<UChestInteractUI>(GetWorld(), InteractWidgetComp->GetWidgetClass());
	if (InteractWidget)
	{
		InteractWidgetComp->SetWidget(InteractWidget);

		InteractWidget->SetCostText(ScrapPrice);
		InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AItemChest::SetPairedPing(APingActor* myPing)
{
	if (MyPing == nullptr)
	{
		MyPing = myPing;
	}
}

bool AItemChest::HasPing()
{
	return (MyPing != nullptr);
}

void AItemChest::Interact()
{
	if (bWasOpened)
		return;

	if (player == nullptr)
		return;

	UE_LOG(LogTemp, Error, TEXT("%s Recieved chest from player"), *GetName());

	player->SetInteractionChest(this);
}

void AItemChest::Server_OpenChest_Implementation(bool bFeelinLucky)
{
	if (HasAuthority())
	{

		if (player == nullptr)
			return;

		player->SetInteractionChest(nullptr);

		if (ScrapPrice > player->GetCurrentScrap())
		{
			UE_LOG(LogTemp, Error, TEXT("Not Enough Scrap"));
			
			if (AudioComp && InvalidScrapAudio)
			{
				AudioComp->SetSound(InvalidScrapAudio);
				AudioComp->Play();
			}

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
			if (MyPing != nullptr)
				MyPing->TimedDestroy();

			FGameplayEffectContextHandle effectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle effectSpecHandle = ASC->MakeOutgoingSpec(ScrapPriceEffect, 1.0f, effectContext);
			FGameplayEffectSpec* spec = effectSpecHandle.Data.Get();

			if (spec)
			{
				spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetScrapTag(), -ScrapPrice);
				ASC->ApplyGameplayEffectSpecToSelf(*spec);
			}


			AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
			if (gameState == GetOwner())
			{
				int amount = bFeelinLucky ? 2 : 1 ;
				gameState->SelectChest(this, amount);
			}
		}
	}
}


void AItemChest::UpdateChestOpened_Implementation()
{
	if (InteractWidget != nullptr)
	{
		InteractWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	/*if (ChestTopMesh != nullptr)
	{
		//ChestTopMesh->SetVisibility(false);
	}*/

	if (AudioComp && ChestOpenAudio)
	{
		AudioComp->SetSound(ChestOpenAudio);
		AudioComp->Play();
	}

	ChestBottomMesh->SetOverlayMaterial(nullptr);
	ChestTopMesh->SetOverlayMaterial(nullptr);

	SphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bWasOpened = true;
	OpenChestAnim();
}

void AItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AItemChest, bWasOpened, COND_None);
	DOREPLIFETIME_CONDITION(AItemChest, MyPing, COND_None);
	DOREPLIFETIME_CONDITION(AItemChest, ScrapPrice, COND_None);
}