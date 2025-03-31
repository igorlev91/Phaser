// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RRightButton.h"
#include "Actors/Clipboard.h"
#include "Player/RCharacterSelectController.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/RCharacterSelectMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

void ARRightButton::DelayTimer()
{
	SetLight(true);
	bPressDelay = false;

	if (DynamicConveyerMaterialInstance)
	{
		DynamicConveyerMaterialInstance->SetVectorParameterValue(FName("Scroll"), FVector(0.0f, 0.0f, 0.0f));
	}
}

void ARRightButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (GEngine && bPressDelay == false)
	{
		if (GameMode)
		{
			// Successfully got the game mode
			SetLight(false);
			bPressDelay = true;

			if (DynamicConveyerMaterialInstance)
			{
				DynamicConveyerMaterialInstance->SetVectorParameterValue(FName("Scroll"), FVector(0.15f, 0.0f, 0.0f));
			}

			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ARRightButton::DelayTimer, 1.3f, false);
			Clipboard->SetNewCharacter(GameMode->NextCharacter());
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RightButton clicked!"));
		}
	}
}

void ARRightButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}

void ARRightButton::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ARCharacterSelectMode>(UGameplayStatics::GetGameMode(GetWorld()));

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Conveyer"), FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(FoundActors[i]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		DynamicConveyerMaterialInstance = UMaterialInstanceDynamic::Create(ConveyerMaterial, MeshComponent);
		if (DynamicConveyerMaterialInstance)
		{
			MeshComponent->SetMaterial(1, DynamicConveyerMaterialInstance);
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClipboard::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		Clipboard = Cast<AClipboard>(FoundActors[0]);
	}
}
