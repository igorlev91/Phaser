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
#include "CineCameraActor.h"
#include "LevelSequence.h"
#include "LevelSequenceCameraSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Widgets/CharacterSelect.h"
#include "Framework/RCharacterDefination.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequence.h"
#include "Widgets/MainMenu.h"

void ARMainMenuController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BP_OnRep_PlayerState();

	if (IsLocalPlayerController()) //maybe also check if they have authority?
	{
		myPlayerID = GetPlayerID();
		//CreateCharacterSelectUI();
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
		break;
	}

	for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)
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

	SetViewTarget(CineCamera);

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	CurrentlyHoveredCharacter = GameState->GetDefinationFromIndex(0);

	GetGameInstance<UEOSGameInstance>()->SetMenuController(this);

	GetGameInstance<UEOSGameInstance>()->SessionJoined.AddUObject(this, &ARMainMenuController::JoinedSession);

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

void ARMainMenuController::ConfirmCharacterChoice()
{
	if (!IsLocalPlayerController())
		return;

	AEOSPlayerState* playerState = Cast<AEOSPlayerState>(PlayerState);
	if (playerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}
	playerState->Server_CharacterSelected(CurrentlyHoveredCharacter);
}

int ARMainMenuController::GetPlayerID()
{
	for (int i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		//AEOSPlayerState* currentPlayer = Cast<AEOSPlayerState>(GameState->PlayerArray[i]);
		//if (myPlayerID == GameState->PlayerArray[i].)
		//{
		//	return i;
		//}
	}

	return 1;
}

void ARMainMenuController::PostPossessionSetup(APawn* NewPawn)
{

}

void ARMainMenuController::JoinedSession()
{
	ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		SequencePlayer->Play();
		SequencePlayer->OnFinished.AddDynamic(this, &ARMainMenuController::OnSequenceEnd);
	}
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
	ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		FMovieSceneSequencePlaybackParams playbackParams;
		playbackParams.Frame = FFrameNumber(70);

		SequencePlayer->SetPlaybackPosition(playbackParams);
		SequencePlayer->Play();

		SequencePlayer->OnFinished.AddDynamic(this, &ARMainMenuController::OnSequenceEnd);
	}
}
