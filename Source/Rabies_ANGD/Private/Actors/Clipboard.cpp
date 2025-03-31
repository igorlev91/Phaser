// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Clipboard.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"

#include "Framework/RCharacterDefination.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Framework/EOSActionGameState.h"
#include "Widgets/ClipboardInfo.h"

// Sets default values
AClipboard::AClipboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClipboardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	ClipboardMesh->SetupAttachment(GetRootComponent());

	RootComponent = ClipboardMesh;

	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	InteractWidgetComp->SetupAttachment(GetRootComponent());
}

void AClipboard::MoveClipboard(FVector goal, FRotator rotationGoal)
{
	FVector CurrentOffset = GetActorLocation();
	FRotator CurrentRotOffset = GetActorRotation();
	if (FVector::Dist(CurrentOffset, goal) < 1)
	{
		SetActorLocation(goal);
		return;
	}

	FVector NewOffset = FMath::Lerp(CurrentOffset, goal, GetWorld()->GetDeltaSeconds() * 8.0f);
	FRotator RotationOffset = FMath::Lerp(CurrentRotOffset, rotationGoal, GetWorld()->GetDeltaSeconds() * 8.0f);
	SetActorLocation(NewOffset);
	SetActorRotation(RotationOffset);
	MoveTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AClipboard::MoveClipboard, goal, rotationGoal));
}

void AClipboard::SetClipboardInfo(URCharacterDefination* Character)
{
	if (infoWidget)
	{
		infoWidget->SetCharacterInfo(Character);
	}
}

void AClipboard::SetNewCharacter(URCharacterDefination* Character)
{
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	MoveClipboard(ClipboardPosHidden, ClipboardRotHidden);
	if (Character)
	{
		CurrentCharacter = Character;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AClipboard::FinishNewCharacter, 1.0f, false);
	}
}

void AClipboard::FinishNewCharacter()
{
	MoveClipboard(ClipboardPosShown, ClipboardRotShown);
	SetClipboardInfo(CurrentCharacter);
}

// Called when the game starts or when spawned
void AClipboard::BeginPlay()
{
	Super::BeginPlay();

	
	infoWidget = Cast<UClipboardInfo>(InteractWidgetComp->GetUserWidgetObject());
	infoWidget->SetVisibility(ESlateVisibility::Visible);

	SetNewCharacter(DefaultCharacter);
}

// Called every frame
void AClipboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

