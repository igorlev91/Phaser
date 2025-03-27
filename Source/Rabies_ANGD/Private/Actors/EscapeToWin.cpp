// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EscapeToWin.h"
#include "Player/RPlayerBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Widgets/CanEscape.h"
#include "Widgets/CannotEscape.h"
#include "Widgets/GameWinUI.h"
#include "Widgets/InitiateBossFight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEscapeToWin::AEscapeToWin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapEnd);

	EndGameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("End Game Mesh"));
	EndGameMesh->SetupAttachment(GetRootComponent());
	EndGameMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	CanEscapeWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Can Escape Widget Comp");
	CanEscapeWidgetComp->SetupAttachment(GetRootComponent());

	CannotEscapeWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Cannot Escape Widget Comp");
	CannotEscapeWidgetComp->SetupAttachment(GetRootComponent());

	GameWinWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Win Game Widget Comp");
	GameWinWidgetComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEscapeToWin::BeginPlay()
{
	Super::BeginPlay();

	SetUpTrueUI();
	SetUpFalseUI();
	SetUpEndUI();
}

// Called every frame
void AEscapeToWin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEscapeToWin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (bHasWonGame || !player)
	{
		return;
	}

	if (bHasBeatenBoss)
	{
		UE_LOG(LogTemp, Warning, TEXT("You have defeated the boss! You can escape!"));
		CanEscapeWidgetComp->SetVisibility(true);

		player->PlayerInteraction.AddUObject(this, &AEscapeToWin::EndGame);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Door is locked, defeat the boss."));
		CannotEscapeWidgetComp->SetVisibility(true);
	}
}

void AEscapeToWin::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (bHasWonGame || !player)
	{
		return;
	}

	if (bHasBeatenBoss)
	{
		CanEscapeWidgetComp->SetVisibility(false);
		player->PlayerInteraction.Clear();
	}
	else
	{
		CannotEscapeWidgetComp->SetVisibility(false);
	}
}

void AEscapeToWin::SetUpTrueUI()
{
	CanEscapeWidgetUI = Cast<UCanEscape>(CanEscapeWidgetComp->GetUserWidgetObject());

	CanEscapeWidgetComp->SetVisibility(false);
}

void AEscapeToWin::SetUpFalseUI()
{
	CannotEscapeWidgetUI = Cast<UCannotEscape>(CannotEscapeWidgetComp->GetUserWidgetObject());

	CannotEscapeWidgetComp->SetVisibility(false);
}

void AEscapeToWin::SetUpEndUI()
{
	GameWinUI = Cast<UGameWinUI>(GameWinWidgetComp->GetUserWidgetObject());
	
	GameWinWidgetComp->SetVisibility(false);
}

void AEscapeToWin::SetUpBossUI()
{
	InitiateBossFightUI = Cast<UInitiateBossFight>(InitiateBossWidgetComp->GetUserWidgetObject());

	InitiateBossWidgetComp->SetVisibility(false);
}

bool AEscapeToWin::SetActivatingExit()
{
	bHasBeatenBoss = true;
	return bHasBeatenBoss;
}

void AEscapeToWin::EndGame()
{
	UE_LOG(LogTemp, Error, TEXT("You Win!!!"));
	bHasWonGame = true;

	CanEscapeWidgetComp->SetVisibility(false);
	player->PlayerInteraction.Clear();

	GameWinWidgetComp->SetVisibility(true);
}
