// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RMainMenuController.h"
#include "Framework/EOSGameInstance.h"
#include "Framework/EOSPlayerState.h"
#include "Framework/EOSGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "Engine/SpotLight.h"                // For ASpotLight
#include "Components/SpotLightComponent.h"
#include "CineCameraActor.h"
#include "LevelSequence.h"
#include "LevelSequenceCameraSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Widgets/CharacterSelect.h"
#include "Framework/RCharacterDefination.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequence.h"
#include "Widgets/MainMenu.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

ARMainMenuController::ARMainMenuController()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
}

void ARMainMenuController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BP_OnRep_PlayerState();

	if (IsLocalPlayerController()) //maybe also check if they have authority?
	{

	}
}

void ARMainMenuController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
}

void ARMainMenuController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

	for (TActorIterator<ACineCameraActor> It(GetWorld()); It; ++It)
	{
		CineCamera = *It;
		SetViewTarget(CineCamera);
		break;
	}

	/*for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)
	{
		MainMenuSequence = *It;
		break;
	}

	if (!CineCamera || !MainMenuSequence)
		return;

	ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		FMovieSceneSequencePlaybackParams playbackParams;
		playbackParams.Time = 0;
		SequencePlayer->SetPlaybackPosition(playbackParams);
	}

	*/

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	GetGameInstance<UEOSGameInstance>()->SetMenuController(this);

	GetGameInstance<UEOSGameInstance>()->SessionJoined.AddUObject(this, &ARMainMenuController::JoinedSession);

	GetWorldTimerManager().SetTimer(BacklightTimerHandle, this, &ARMainMenuController::EnableBacklights, 1.5f, false);
	GetWorldTimerManager().SetTimer(FrontlightTimerHandle, this, &ARMainMenuController::EnableFrontlights, 2.5f, false);
	GetWorldTimerManager().SetTimer(FinalTimerHandle, this, &ARMainMenuController::EnableComputer, 3.5f, false);
}

void ARMainMenuController::EnableLights(UWorld* world, FName Tag)
{
	if (!world) return;

	for (TActorIterator<AActor> It(world); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->Tags.Contains(Tag))
		{
			ASpotLight* SpotLight = Cast<ASpotLight>(Actor);
			if (SpotLight)
			{
				SpotLight->GetLightComponent()->SetVisibility(true);
			}
		}
	}
}

void ARMainMenuController::EnableBacklights()
{
	if (AudioComp && LightsOnAudio)
	{
		AudioComp->SetSound(LightsOnAudio);
		AudioComp->Play();
	}
	EnableLights(GetWorld(), "backlight");
}

void ARMainMenuController::EnableFrontlights()
{
	EnableLights(GetWorld(), "frontlight");
}

void ARMainMenuController::EnableComputer()
{
	if (AudioComp && MonitorLightAudio)
	{
		AudioComp->SetSound(MonitorLightAudio);
		AudioComp->Play();
	}

	FName monitorTag = "Monitor";
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(monitorTag))
		{
			UStaticMeshComponent* MeshComponent = It->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComponent)
			{
				// Set the material at the specified index
				MeshComponent->SetMaterial(1, ComputerMaterial);
			}
		}
	}

	CreateMenuUI();
}

void ARMainMenuController::ChangeOnlineMenuState(bool state)
{
	if (MenuUI)
	{
		MenuUI->ChangeConnectMenuState(state);
	}
}

void ARMainMenuController::ChangeMainMenuState(bool state)
{
	if (MenuUI)
	{
		MenuUI->ChangeMainMenuState(state);
	}
}

void ARMainMenuController::PostPossessionSetup(APawn* NewPawn)
{

}

void ARMainMenuController::JoinedSession()
{
	/*ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		SequencePlayer->Play();
		SequencePlayer->OnFinished.AddDynamic(this, &ARMainMenuController::OnSequenceEnd);
	}*/
}

void ARMainMenuController::CreateMenuUI()
{
	if (!MenuUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	MenuUI = CreateWidget<UMainMenu>(this, MenuUIClass);
	if (MenuUI)
	{
		MenuUI->AddToViewport();
	}
}

void ARMainMenuController::OnSequenceEnd()
{
	/*ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		FMovieSceneSequencePlaybackParams playbackParams;
		playbackParams.Frame = FFrameNumber(40);

		SequencePlayer->SetPlaybackPosition(playbackParams);

		//SequencePlayer->OnFinished.AddDynamic(this, &ARMainMenuController::OnSequenceEnd);
	}*/
}
