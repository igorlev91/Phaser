// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RCharacterSelectController.h"
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
#include "Net/UnrealNetwork.h"
#include "LevelSequenceCameraSettings.h"
#include "GameFramework/PlayerController.h"
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

void ARCharacterSelectController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BP_OnRep_PlayerState();
	if (IsLocalController() && CharacterSelectUI == nullptr) //maybe also check if they have authority?
	{
		GetCameraView();
		CreateCharacterSelectUI();

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

		MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
		if (MyPlayerState == nullptr)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found on rep player state!"));
			}
			return;
		}

		if (!MyPlayerState->OnHoveredCharacterIndexReplicated.IsAlreadyBound(this, &ARCharacterSelectController::HoveredCharacterIndexChange))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Binded on character index"));
			MyPlayerState->OnHoveredCharacterIndexReplicated.AddDynamic(this, &ARCharacterSelectController::HoveredCharacterIndexChange);
		}
	}
}

void ARCharacterSelectController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
}

void ARCharacterSelectController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARCharacterSelectController::BeginPlay()
{
	Super::BeginPlay();

	bAutoManageActiveCameraTarget = false;

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

	GetCagedCharacters();
	GetCameraView();

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClipboard::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		Clipboard = Cast<AClipboard>(FoundActors[0]);
	}

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	/*MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
	if (MyPlayerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found on begin play!"));
		}
		return;
	}

	if (!MyPlayerState->OnHoveredCharacterIndexReplicated.IsAlreadyBound(this, &ARCharacterSelectController::HoveredCharacterIndexChange))
	{
		MyPlayerState->OnHoveredCharacterIndexReplicated.AddDynamic(this, &ARCharacterSelectController::HoveredCharacterIndexChange);
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

		SequencePlayer->Play();
		SequencePlayer->OnFinished.AddDynamic(this, &ARCharacterSelectController::OnSequenceEnd);
	}*/

	//CurrentlyHoveredCharacter = GameState->GetDefinationFromIndex(0);
}

void ARCharacterSelectController::ConfirmCharacterChoice()
{
	if (!IsLocalPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is Not local player"));
		return;
	}

	if (MyPlayerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}

	ServerRequestButtonClick(MyPlayerState);
}

void ARCharacterSelectController::NextCharacter(class ARRightButton* rightButton)
{
	RightButton = rightButton;

	if (!IsLocalPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is Not local player"));
		return;
	}

	if (MyPlayerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}
	ServerRequestNextClick(MyPlayerState);
}

void ARCharacterSelectController::ServerRequestButtonClick_Implementation(AEOSPlayerState* requestingPlayerState)
{
	requestingPlayerState->Server_IssueCharacterPick_Implementation(GameState->GetDefinationFromIndex(requestingPlayerState->HoveredCharacterIndex));
	GameState->Server_ReadyUp();
}

void ARCharacterSelectController::ServerRequestNextClick_Implementation(AEOSPlayerState* requestingPlayerState)
{
	requestingPlayerState->Server_ChangeHoveredCharacterPick();
}

void ARCharacterSelectController::GetCameraView()
{
	/*for (TActorIterator<AActor> It(GetWorld()); It; ++It)
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
	}*/

	for (TActorIterator<ACineCameraActor> It(GetWorld()); It; ++It)
	{
		CineCamera = *It;
		SetViewTarget(CineCamera);
		break;
	}
}

void ARCharacterSelectController::PostPossessionSetup(APawn* NewPawn)
{
	if (IsLocalController() && CharacterSelectUI == nullptr) //maybe also check if they have authority?
	{
		CreateCharacterSelectUI();
	}
}

void ARCharacterSelectController::CreateCharacterSelectUI()
{
	if (!CharacterSelectUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	CharacterSelectUI = CreateWidget<UCharacterSelect>(this, CharacterSelectUIClass);
	if (CharacterSelectUI)
	{
		CharacterSelectUI->AddToViewport();
	}
}

void ARCharacterSelectController::OnSequenceEnd()
{
	/*ULevelSequencePlayer* SequencePlayer = MainMenuSequence->GetSequencePlayer();
	if (SequencePlayer)
	{
		FMovieSceneSequencePlaybackParams playbackParams;
		playbackParams.Frame = FFrameNumber(29);

		SequencePlayer->SetPlaybackPosition(playbackParams);

		//SequencePlayer->OnFinished.AddDynamic(this, &ARMainMenuController::OnSequenceEnd);
	}*/
}

void ARCharacterSelectController::HoveredCharacterIndexChange(int newIndex)
{
	if (DynamicConveyerMaterialInstance)
	{
		DynamicConveyerMaterialInstance->SetVectorParameterValue(FName("Scroll"), FVector(0.2f, 0.0f, 0.0f));
	}

	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ARCharacterSelectController::EnableClickButton, 1.3f, false);

	GetNextCharacterCage();
	Clipboard->SetNewCharacter(GameState->GetDefinationFromIndex(MyPlayerState->HoveredCharacterIndex));
}

void ARCharacterSelectController::EnableClickButton()
{
	if (RightButton)
	{
		RightButton->EnableClick();
	}

	if (DynamicConveyerMaterialInstance)
	{
		DynamicConveyerMaterialInstance->SetVectorParameterValue(FName("Scroll"), FVector(0.0f, 0.0f, 0.0f));
	}
}


void ARCharacterSelectController::GetNextCharacterCage()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Getting next character"));
	CagedCharacters[0]->TickPosition(OffScreen, Sideline, true);

	if (CagedCharacters.Num() > 1) // Only rotate if there's more than one element
	{
		ACagedCharacter* FirstElement = CagedCharacters[0];
		CagedCharacters.RemoveAt(0);
		CagedCharacters.Add(FirstElement);
	}

	CagedCharacters[0]->TickPosition(ShownCage, Sideline, false);

	//Cast<ARCharacterSelectController>(GetWorld()->GetFirstPlayerController())->SetCurrentlyHoveredCharacter(CagedCharacters[0]->Character);
	//return CagedCharacters[0]->Character;
}

void ARCharacterSelectController::GetCagedCharacters()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACagedCharacter::StaticClass(), FoundActors);
	CagedCharacters.Empty();

	for (AActor* Actor : FoundActors)
	{
		ACagedCharacter* CagedCharacter = Cast<ACagedCharacter>(Actor);
		if (CagedCharacter)
		{
			CagedCharacters.Add(CagedCharacter);
			UE_LOG(LogTemp, Warning, TEXT("CagedCharacter: %s"), *CagedCharacter->GetName());
		}
	}

	Algo::Sort(CagedCharacters, [](const ACagedCharacter* A, const ACagedCharacter* B)
		{
			return A->CharacterIndex < B->CharacterIndex; // Sort in ascending order
		});
}
