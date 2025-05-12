// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RCharacterSelectController.h"
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
#include "Widgets/LoadingScreenUI.h"

#include "Sound/SoundCue.h"

ARCharacterSelectController::ARCharacterSelectController()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;
}

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

		MyPlayerState->OnLoadNewScene.AddUObject(this, &ARCharacterSelectController::CreateLoadingScreenUI);

		if (!MyPlayerState->OnHoveredCharacterIndexReplicated.IsAlreadyBound(this, &ARCharacterSelectController::HoveredCharacterIndexChange))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Binded on character index"));
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

	FInputModeGameAndUI input;
	SetInputMode(input);
	bEnableMouseOverEvents = true;

	//GetWorld()->GetFirstPlayerController()->SetInputMode(input);

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
		if (Clipboard)
		{
			if (Clipboard->infoWidget)
			{
				Clipboard->infoWidget->meleeIcon->OnMediaPlayerHovered.RemoveAll(this);
				Clipboard->infoWidget->rangedIcon->OnMediaPlayerHovered.RemoveAll(this);
				Clipboard->infoWidget->specialIcon->OnMediaPlayerHovered.RemoveAll(this);
				Clipboard->infoWidget->ultimateIcon->OnMediaPlayerHovered.RemoveAll(this);
				Clipboard->infoWidget->passiveIcon->OnMediaPlayerHovered.RemoveAll(this);

				Clipboard->infoWidget->meleeIcon->OnMediaPlayerHovered.AddUObject(this, &ARCharacterSelectController::PlayMedia);
				Clipboard->infoWidget->rangedIcon->OnMediaPlayerHovered.AddUObject(this, &ARCharacterSelectController::PlayMedia);
				Clipboard->infoWidget->specialIcon->OnMediaPlayerHovered.AddUObject(this, &ARCharacterSelectController::PlayMedia);
				Clipboard->infoWidget->ultimateIcon->OnMediaPlayerHovered.AddUObject(this, &ARCharacterSelectController::PlayMedia);
				Clipboard->infoWidget->passiveIcon->OnMediaPlayerHovered.AddUObject(this, &ARCharacterSelectController::PlayMedia);

				Clipboard->infoWidget->meleeIcon->OnMediaPlayerLeft.AddUObject(this, &ARCharacterSelectController::StopMedia);
				Clipboard->infoWidget->rangedIcon->OnMediaPlayerLeft.AddUObject(this, &ARCharacterSelectController::StopMedia);
				Clipboard->infoWidget->specialIcon->OnMediaPlayerLeft.AddUObject(this, &ARCharacterSelectController::StopMedia);
				Clipboard->infoWidget->ultimateIcon ->OnMediaPlayerLeft.AddUObject(this, &ARCharacterSelectController::StopMedia);
				Clipboard->infoWidget->passiveIcon->OnMediaPlayerLeft.AddUObject(this, &ARCharacterSelectController::StopMedia);
			}
		}
	}

	GameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	if (GetNetMode() != ENetMode::NM_Standalone)
	{
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character select controller: Online"));
		}
		return;
	}

	MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
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
}

void ARCharacterSelectController::PlayMedia(UFileMediaSource* mediaSource)
{
	if (mediaSource)
	{
		PlayMediaFile(mediaSource);
	}
}

void ARCharacterSelectController::StopMedia()
{
	StopMediaFile();
}

void ARCharacterSelectController::ConfirmCharacterChoice()
{
	if (!IsLocalPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is Not local player"));
		return;
	}

	MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
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

void ARCharacterSelectController::SecretChoice()
{
	if (!IsLocalPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is Not local player"));
		return;
	}

	MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
	if (MyPlayerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}

	ServerRequestSecretClick(MyPlayerState);
}

void ARCharacterSelectController::NextCharacter(class ARRightButton* rightButton)
{
	RightButton = rightButton;

	if (!IsLocalPlayerController())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Is Not local player"));
		return;
	}

	MyPlayerState = Cast<AEOSPlayerState>(PlayerState);
	if (MyPlayerState == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Playerstate was found!"));
		}
		return;
	}
	ServerRequestNextClick(MyPlayerState);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GameState->GetDefinationFromIndex(MyPlayerState->HoveredCharacterIndex)->PickedSoundEffect, FVector(-2865, 833, 761), FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, nullptr);
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

void ARCharacterSelectController::ServerRequestSecretClick_Implementation(AEOSPlayerState* requestingPlayerState)
{
	requestingPlayerState->Server_IssueCharacterPick_Implementation(GameState->SecretCharacter);
	GameState->Server_ReadyUp();
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
		CharacterSelectUI->SetOwningCharacterController(this);
	}
	//CreateLoadingScreenUI();
}

void ARCharacterSelectController::CreateLoadingScreenUI()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	if (!LoadingScreenUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	LoadingScreenUI = CreateWidget<ULoadingScreenUI>(this, LoadingScreenUIClass);
	if (LoadingScreenUI)
	{
		LoadingScreenUI->AddToViewport();
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
		DynamicConveyerMaterialInstance->SetVectorParameterValue(FName("Scroll"), FVector(0.4f, 0.0f, 0.0f));
	}

	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &ARCharacterSelectController::EnableClickButton, 0.7f, false);

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
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Getting next character"));
	CagedCharacters[0]->TickPosition(OffScreen, Sideline, true);

	if (AudioComp && ConveyorAudio)
	{
		AudioComp->SetSound(ConveyorAudio);
		AudioComp->Play();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARCharacterSelectController::PlayConveyorStop, 0.8f, false);
	}

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

void ARCharacterSelectController::SetSettingsState_Implementation(bool state)
{
	if (CharacterSelectUI)
	{
		CharacterSelectUI->SetSettingsState(state);
	}
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

void ARCharacterSelectController::PlayConveyorStop()
{
	if (AudioComp && ConveyorStopAudio)
	{
		AudioComp->SetSound(ConveyorStopAudio);
		AudioComp->Play();
	}
}
