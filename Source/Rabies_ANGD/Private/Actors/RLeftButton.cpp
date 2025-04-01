// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RLeftButton.h"
#include "Player/RCharacterSelectController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/RecordingUI.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundCue.h"

ARLeftButton::ARLeftButton()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
}

void ARLeftButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (AudioComp && SelectAudio && bPlaySound)
	{
		UE_LOG(LogTemp, Error, TEXT("Play Game"));
		AudioComp->SetSound(SelectAudio);
		AudioComp->Play();
		bPlaySound = false;
	}

	if (GEngine && CharacterSelectController && bSelected == false)
	{
		bSelected = true;
		SetLight(false);
		CharacterSelectController->ConfirmCharacterChoice();

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RecordingUI", FoundActors);
		if (FoundActors[0])
		{
			UWidgetComponent* WidgetComp = FoundActors[0]->FindComponentByClass<UWidgetComponent>();
			if (WidgetComp)
			{
				URecordingUI* recordingUI = Cast<URecordingUI>(WidgetComp->GetUserWidgetObject());
				if (recordingUI)
				{
					recordingUI->DisplayError();
				}
			}
		}

		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ComputerScreen", FoundActors);
		if (FoundActors[0])
		{
			UMeshComponent* ComputerScreen = FoundActors[0]->FindComponentByClass<UMeshComponent>();
			if (ComputerScreen)
			{
				ComputerScreen->SetMaterial(1, ErrorMaterialInstance);
			}
		}

	}
}

void ARLeftButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}
