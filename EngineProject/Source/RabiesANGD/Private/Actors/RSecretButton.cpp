// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RSecretButton.h"
#include "Player/RCharacterSelectController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Widgets/RecordingUI.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundCue.h"
#include "Actors/RLeftButton.h"

ARSecretButton::ARSecretButton()
{
}

void ARSecretButton::OnActorClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	/*if (AudioComp && SelectAudio && bPlaySound)
	{
		UE_LOG(LogTemp, Error, TEXT("Play Game"));
		AudioComp->SetSound(SelectAudio);
		AudioComp->Play();
		bPlaySound = false;
	}*/
	if (GEngine && CharacterSelectController && bSelected == false)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RedButton", FoundActors);
		if (FoundActors[0])
		{
			ARLeftButton* leftButt = Cast<ARLeftButton>(FoundActors[0]);
			if (leftButt)
			{
				leftButt->DoConfirmCharacter();
			}
		}

		bSelected = true;
		CharacterSelectController->SecretChoice();
	}
}

void ARSecretButton::NotifyActorOnClicked(FKey ButtonPressed)
{
	OnActorClicked(this, ButtonPressed);
}
