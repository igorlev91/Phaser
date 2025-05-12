// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Clipboard.h"
#include "Player/RPlayerBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"

#include "Framework/RCharacterDefination.h"
#include "Framework/AbilityHitBox.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Framework/EOSActionGameState.h"
#include "Widgets/ClipboardInfo.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"


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

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
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

	FVector NewOffset = FMath::Lerp(CurrentOffset, goal, GetWorld()->GetDeltaSeconds() * 13.0f);
	FRotator RotationOffset = FMath::Lerp(CurrentRotOffset, rotationGoal, GetWorld()->GetDeltaSeconds() * 13.0f);
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
	if (AudioComp != nullptr && MoveAudio != nullptr)
	{
		AudioComp->SetSound(MoveAudio);
		AudioComp->Play();
	}

	if (GetWorld() == nullptr)
		return;

	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	MoveClipboard(ClipboardPosHidden, ClipboardRotHidden);
	if (Character)
	{
		CurrentCharacter = Character;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AClipboard::FinishNewCharacter, 0.5f, false);


		//
	}
}

void AClipboard::OpenMovie(int index)
{
	if (infoWidget)
	{
		if (RecordingActor)
		{
			RecordingActor->SetActorHiddenInGame(true);
		}
		if (ComputerScreen)
		{
			ComputerScreen->SetMaterial(1, StaticMaterialInstance);
		}

		infoWidget->PlayMoive(index);
		GetWorldTimerManager().SetTimer(DelayShow, this, &AClipboard::DelayedShow, 0.2f, false);
	}
}

void AClipboard::CloseMovie()
{
	if (RecordingActor && MonitorPreview)
	{
		GetWorldTimerManager().ClearTimer(DelayShow);
		RecordingActor->SetActorHiddenInGame(false);
		MonitorPreview->SetActorHiddenInGame(true);
	}
	if (ComputerScreen)
	{
		ComputerScreen->SetMaterial(1, DefaultMaterialInstance);
	}
}

void AClipboard::DelayedShow()
{
	if (MonitorPreview)
	{
		MonitorPreview->SetActorHiddenInGame(false);
	}
}

void AClipboard::FinishNewCharacter()
{
	if (AudioComp != nullptr && MoveAudio != nullptr)
	{
		AudioComp->SetSound(MoveAudio);
		AudioComp->Play();
	}
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	MoveClipboard(ClipboardPosShown, ClipboardRotShown);
	SetClipboardInfo(CurrentCharacter);
}

// Called when the game starts or when spawned
void AClipboard::BeginPlay()
{
	Super::BeginPlay();

	if (InteractWidgetComp != nullptr)
	{
		infoWidget = Cast<UClipboardInfo>(InteractWidgetComp->GetUserWidgetObject());

		if (infoWidget != nullptr)
		{
			infoWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	if(DefaultCharacter != nullptr)
		SetNewCharacter(DefaultCharacter);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PreviewScreen"), FoundActors);
	if (FoundActors[0])
	{
		MonitorPreview = FoundActors[0];
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RecordingUI", FoundActors);
	if (FoundActors[0])
	{
		RecordingActor = FoundActors[0];
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ComputerScreen", FoundActors);
	if (FoundActors[0])
	{
		ComputerScreen = FoundActors[0]->FindComponentByClass<UMeshComponent>();
	}

	if (HitBoxClass == nullptr)
		return;

	MeleeHitBox = GetWorld()->SpawnActor<AAbilityHitBox>(HitBoxClass);
	if (MeleeHitBox)
	{
		MeleeHitBox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		MeleeHitBox->SetActorRelativeLocation(FVector(-32.902772, -4.27744, 9.333333));
		MeleeHitBox->SetIndex(0);
	}

	RangedHitBox = GetWorld()->SpawnActor<AAbilityHitBox>(HitBoxClass);
	if (RangedHitBox)
	{
		RangedHitBox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		RangedHitBox->SetActorRelativeLocation(FVector(-12.249889, -3.525216, 9.272541));
		RangedHitBox->SetIndex(1);
	}

	SpecialHitBox = GetWorld()->SpawnActor<AAbilityHitBox>(HitBoxClass);
	if (SpecialHitBox)
	{
		SpecialHitBox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		SpecialHitBox->SetActorRelativeLocation(FVector(8.092985, -4.149483, 9.272541));
		SpecialHitBox->SetIndex(2);
	}

	UltimateHitBox = GetWorld()->SpawnActor<AAbilityHitBox>(HitBoxClass);
	if (UltimateHitBox)
	{
		UltimateHitBox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		UltimateHitBox->SetActorRelativeLocation(FVector(28.435859, -4.77375, 9.272541));
		UltimateHitBox->SetIndex(3);
	}

	PassiveHitBox = GetWorld()->SpawnActor<AAbilityHitBox>(HitBoxClass);
	if (PassiveHitBox)
	{
		PassiveHitBox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		PassiveHitBox->SetActorRelativeLocation(FVector(32.06618, -0.384534, -22.060792));
		PassiveHitBox->SetIndex(4);
	}
}

// Called every frame
void AClipboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

