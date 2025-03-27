// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EscapeToWin.h"
#include "Player/RPlayerBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Widgets/EndGameWidget.h"
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

	EndGameWidgetComp = CreateDefaultSubobject<UWidgetComponent>("End Game Widget Comp");
	EndGameWidgetComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AEscapeToWin::BeginPlay()
{
	Super::BeginPlay();

	SetUpEndGame();
}

// Called every frame
void AEscapeToWin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEscapeToWin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player || !player->IsLocallyControlled() || bHasWonGame)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Try to Escape?"));
	EndGameUI->SetVisibility(ESlateVisibility::Visible);
	EndGameUI->UpdateText(FText::FromString("Insert Keycard?\n[F]"));
	EndGameUI->UpdateTextColor(FLinearColor::White);

	if (!bStartBoss)
	{
		player->PlayerInteraction.AddUObject(this, &AEscapeToWin::CheckKeyCard);
		return;
	}
	else
	{
		player->PlayerInteraction.AddUObject(this, &AEscapeToWin::UseKeycard);
		return;
	}
}

void AEscapeToWin::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player || !player->IsLocallyControlled() || bHasWonGame)
	{
		return;
	}

	EndGameUI->SetVisibility(ESlateVisibility::Collapsed);
	player->PlayerInteraction.Clear();
}

void AEscapeToWin::SetUpEndGame()
{
	EndGameUI = Cast<UEndGameWidget>(EndGameWidgetComp->GetUserWidgetObject());
	EndGameUI->SetVisibility(ESlateVisibility::Collapsed);
}

void AEscapeToWin::CheckKeyCard()
{
	//Check to see if player has keycard

	if (bHasKeyCard == true)
	{
		SpawnBoss();
	}
	else
	{
		EndGameUI->UpdateText(FText::FromString("No Keycard Detected.\nAccessed Denied."));
		EndGameUI->UpdateTextColor(FLinearColor::Red);
	}
}

void AEscapeToWin::SpawnBoss()
{
	UE_LOG(LogTemp, Error, TEXT("Spawning Boss"));
	bStartBoss = true;

	player->PlayerInteraction.Clear();

	EndGameUI->UpdateText(FText::FromString("ERROR!\nAccess Overrided."));
	EndGameUI->UpdateTextColor(FLinearColor::Red);
	
	//Spawn Boss into the world
}

void AEscapeToWin::UseKeycard()
{
	if (!bHasBeatenBoss)
	{
		UE_LOG(LogTemp, Error, TEXT("Door is locked, defeat the boss."));
		EndGameUI->UpdateText(FText::FromString("Access Denied.\nDeadlock Security - Online"));
		EndGameUI->UpdateTextColor(FLinearColor::Red);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Door is opened, you escaped!"));
		EndGameUI->UpdateText(FText::FromString("Access Granted.\nLeave the Facility?\n[F]"));
		EndGameUI->UpdateTextColor(FLinearColor::Green);
		
		player->PlayerInteraction.Clear();
		player->PlayerInteraction.AddUObject(this, &AEscapeToWin::EndGame);
	}
}

void AEscapeToWin::SetActivatingExit()
{
	bHasBeatenBoss = true;
}

void AEscapeToWin::EndGame()
{
	UE_LOG(LogTemp, Error, TEXT("You Win!!!"));
	bHasWonGame = true;

	player->PlayerInteraction.Clear();

	//Whatever needs to be done for the ending, I am not sure exactly what they want
	EndGameUI->UpdateText(FText::FromString("CONGRATULATIONS!\nYou have beaten the game!"));
	EndGameUI->UpdateTextColor(FLinearColor::White);
}
