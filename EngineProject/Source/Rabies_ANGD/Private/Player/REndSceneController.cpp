// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/REndSceneController.h"
#include "Framework/EOSGameInstance.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/EOSGameState.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "CineCameraActor.h"
#include "LevelSequence.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequenceCameraSettings.h"
#include "Actors/RRightButton.h"
#include "GameFramework/GameStateBase.h"
#include "Widgets/CharacterSelect.h"
#include "Framework/RCharacterDefination.h"
#include "Actors/CagedCharacter.h"
#include "Actors/Clipboard.h"
#include "Algo/Sort.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequence.h"
#include "Framework/RCharacterDefination.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/ClipboardInfo.h"
#include "Widgets/SelectAbilityIcon.h"
#include "IMediaModule.h"
#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "Widgets/SkipCutsceneUI.h"

#include "Sound/SoundCue.h"

AREndSceneController::AREndSceneController()
{
}

void AREndSceneController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	PostPossessionSetup(NewPawn);
}

void AREndSceneController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void AREndSceneController::PostPossessionSetup(APawn* NewPawn)
{

}

void AREndSceneController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Ensure the InputComponent exists
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AREndSceneController::OnSkipCutscene);
		InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AREndSceneController::OnSkipCutscene);
		InputComponent->BindKey(EKeys::BackSpace, IE_Pressed, this, &AREndSceneController::OnSkipCutscene);
	}
}

void AREndSceneController::GetCameraView()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->Tags.Contains("MainCamera"))
		{
			ACineCameraActor* cameraActor = Cast<ACineCameraActor>(Actor);
			if (cameraActor)
			{
				CineCamera = cameraActor;
				SetViewTarget(CineCamera);
			}
		}
	}

	for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag("EndCutscene"))
		{
			EndCutsceneSequence = *It;
			break;
		}
	}

	if (!EndCutsceneSequence)
	{
		UE_LOG(LogTemp, Error, TEXT("EndCutsceneSequence is null! Make sure it's placed in the level and tagged correctly."));
		return;
	}

	ULevelSequencePlayer* SequencePlayer = EndCutsceneSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		SequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(FFrameNumber(0), EUpdatePositionMethod::Play));
		SequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LevelSequencePlayer is null. Was the Level Sequence asset assigned to the LevelSequenceActor?"));
	}

	GetWorldTimerManager().SetTimer(endCutsceneHandle, this, &AREndSceneController::OnSkipCutscene, 63, false);

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AREndSceneController::OnSkipCutscene()
{
	if (MainMenu.ToSoftObjectPath().IsValid())
	{
		FString MapPath = MainMenu.ToSoftObjectPath().ToString();
		FString CleanMapName = FPackageName::ObjectPathToPackageName(MapPath);
		UGameplayStatics::OpenLevel(this, FName(*CleanMapName));
	}

}


void AREndSceneController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("%s,  Got to begin play"), *GetName());
	CreateUI();
	bAutoManageActiveCameraTarget = false;
	GetCameraView();
}

void AREndSceneController::CreateUI()
{
	if (!SkipScreenUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	SkipScreenUI = CreateWidget<USkipCutsceneUI>(this, SkipScreenUIClass);
	if (SkipScreenUI)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  Created loading screen UI"), *GetName());
		SkipScreenUI->AddToViewport();
	}
}