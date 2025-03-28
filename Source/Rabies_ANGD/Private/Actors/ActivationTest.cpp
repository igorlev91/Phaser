// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ActivationTest.h"
#include "Components/SphereComponent.h"
#include "Actors/EscapeToWin.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Player/RPlayerBase.h"

// Sets default values
AActivationTest::AActivationTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AActivationTest::OnOverlapBegin);

	ActivationWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Activation Widget Comp");
	ActivationWidgetComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AActivationTest::BeginPlay()
{
	Super::BeginPlay();
	
	SetUp();
}

// Called every frame
void AActivationTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActivationTest::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player || !player->IsLocallyControlled())
	{
		return;
	}

	if (bActivateCard && !bDefeatBoss)
	{
		SetActivationCard();
	}
	else if (!bActivateCard && bDefeatBoss)
	{
		SetDefeatBoss();
	}
}

void AActivationTest::SetUp()
{
	EscapeGame = Cast<AEscapeToWin>(UGameplayStatics::GetActorOfClass(GetWorld(), AEscapeToWin::StaticClass()));

	if (EscapeGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("We have the EndGame!"));
	}
}

void AActivationTest::SetActivationCard()
{
	EscapeGame->bHasKeyCard = true;
}

void AActivationTest::SetDefeatBoss()
{
	EscapeGame->bHasBeatenBoss = true;
}

