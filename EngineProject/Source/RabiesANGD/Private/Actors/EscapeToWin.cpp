// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EscapeToWin.h"
#include "Player/RPlayerBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Character/RCharacterBase.h"
#include "Widgets/EndGameWidget.h"
#include "Sound/SoundCue.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/EOSActionGameState.h"
#include "Music/AS_BackgroundMusic.h"
#include "EngineUtils.h"

// Sets default values
AEscapeToWin::AEscapeToWin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	//SetReplicates(true);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AEscapeToWin::OnOverlapEnd);

	EndGameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("End Game Mesh"));
	EndGameMesh->SetupAttachment(GetRootComponent());
	EndGameMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	EndGameWidgetComp = CreateDefaultSubobject<UWidgetComponent>("End Game Widget Comp");
	EndGameWidgetComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;

}

void AEscapeToWin::Interact()
{
	if (player == nullptr)
		return;

	player->SetInteractionWin(this);
}

// Called when the game starts or when spawned
void AEscapeToWin::BeginPlay()
{
	Super::BeginPlay();

	SetUpEndGame();
}

void AEscapeToWin::OnRep_CurrentText()
{
	EndGameUI->UpdateText(FText::FromString(currentUIText));
}

void AEscapeToWin::OnRep_CurrentColor()
{
	EndGameUI->UpdateTextColor(currentUIColor);
}

void AEscapeToWin::ChangeText_Implementation(const FString& newText, FLinearColor newColor)
{
	if (EndGameUI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No UI"));
		return;
	}
	currentUIText = newText;
	currentUIColor = newColor;
	EndGameUI->UpdateText(FText::FromString(currentUIText));
	EndGameUI->UpdateTextColor(currentUIColor);
}

// Called every frame
void AEscapeToWin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEscapeToWin::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	player = Cast<ARPlayerBase>(OtherActor);

	if (!player || !player->IsLocallyControlled() || bHasWonGame)
	{
		return;
	}

	if(EndGameUI)
		EndGameUI->SetVisibility(ESlateVisibility::Visible);

	ChangeText("Insert Keycard?\n[F]", FLinearColor::White);

	canPlay = true;

	UE_LOG(LogTemp, Warning, TEXT("canPlay: %s"), canPlay ? TEXT("True") : TEXT("False"));
	
	player->SetInteractionWin(this);
	player->PlayerInteraction.AddUObject(this, &AEscapeToWin::Interact);
}

void AEscapeToWin::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (player)
	{
		player->PlayerInteraction.Clear();
		player->SetInteractionWin(nullptr);

	}

	if (!player || !player->IsLocallyControlled() || bHasWonGame)
	{
		return;
	}

	//player = nullptr;

	if (EndGameUI)
		EndGameUI->SetVisibility(ESlateVisibility::Collapsed);

}

void AEscapeToWin::SetUpEndGame_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Set up"));
	if (EndGameWidgetComp == nullptr)
		return;
	EndGameUI = Cast<UEndGameWidget>(EndGameWidgetComp->GetUserWidgetObject());

	if (EndGameUI == nullptr)
		return;
	EndGameUI->SetVisibility(ESlateVisibility::Collapsed);
}

void AEscapeToWin::CheckKeyCard_Implementation()
{
	//Check to see if player has keycard
	if (GetWorld() == nullptr)
	{
		return;
	}

	AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
	if (!gameState)
		return;

	if(gameState->PlayerArray.Num() <= 0)
		return;


	if (bHasKeyCard == false)
	{
		for (APlayerState* playerState : gameState->PlayerArray)
		{
			if (playerState)
			{
				if (playerState->GetPawn())
				{
					ARPlayerBase* checkingPlayer = Cast<ARPlayerBase>(playerState->GetPawn());
					if (checkingPlayer)
					{
						if (checkingPlayer->GetKeyCard())
						{
							bHasKeyCard = true;
							break;
						}
					}
				}
			}
		}
	}


	if (bHasKeyCard == true && GetWorld() != nullptr)
	{
		for (TActorIterator<AAS_BackgroundMusic> BackgroundMusicInstanceIter = TActorIterator<AAS_BackgroundMusic>(GetWorld()); BackgroundMusicInstanceIter; ++BackgroundMusicInstanceIter)
		{
			if (*BackgroundMusicInstanceIter)
			{
				BackgroundMusic = *BackgroundMusicInstanceIter;
				BackgroundMusic->SwapPlayingMusic();
				break;
			}
		}
		gameState->StartBossFight(0);
		bStartBoss = true;

		if (player)
		{
			player->PlayerInteraction.Clear();
			player->SetInteractionWin(nullptr);
		}

		ChangeText("ERROR!\nAccess Overrided.", FLinearColor::Red);

		if (AudioComp && BossSpawnAudio && canPlay)
		{
			AudioComp->SetSound(BossSpawnAudio);
			AudioComp->Play();
		}

	}
	else if(GetWorld() != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No keycard"));
		ChangeText("No Keycard Detected.\nAccessed Denied.", FLinearColor::Red);

		if (AudioComp && NoCardAudio && canPlay)
		{
			AudioComp->SetSound(NoCardAudio);
			AudioComp->Play();
		}

		canPlay = false;
	}
}

void AEscapeToWin::UseKeycard_Implementation()
{
	if (hasBeatenBoss <= 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Door is locked, defeat the boss."));
		ChangeText("Access Denied.\nDeadlock Security - Online", FLinearColor::Red);

		if (AudioComp && BossNotDefeatedAudio && canPlay)
		{
			AudioComp->SetSound(BossNotDefeatedAudio);
			AudioComp->Play();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Door is opened, you escaped!"));
		ChangeText("Access Granted.\nLeaveing the Facility\n", FLinearColor::Green);

		if (AudioComp && BossDefeatedAudio && canPlay)
		{
			AudioComp->SetSound(BossDefeatedAudio);
			AudioComp->Play();
		}

		AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
		if (!gameState)
			return;

		if (HasAuthority() && GetOwner() == gameState)
		{
			gameState->LeaveLevel();
		}
		
		if (player)
		{
			player->PlayerInteraction.Clear();
			player->SetInteractionWin(nullptr);
		}
		//player->PlayerInteraction.AddUObject(this, &AEscapeToWin::EndGame);
	}

	canPlay = false;
}



void AEscapeToWin::SetActivatingExit()
{
	hasBeatenBoss = 0;
}

void AEscapeToWin::EndGame()
{
	UE_LOG(LogTemp, Error, TEXT("You Win!!!"));
	bHasWonGame = true;

	if (player)
	{
		player->PlayerInteraction.Clear();
		player->SetInteractionWin(nullptr);
	}

	//Whatever needs to be done for the ending, I am not sure exactly what they want
	ChangeText("CONGRATULATIONS!\nYou have beaten the game!", FLinearColor::White);
}

void AEscapeToWin::UpdateEscapeToWin_Implementation()
{
	hasBeatenBoss += 1;
}

void AEscapeToWin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AEscapeToWin, hasBeatenBoss, COND_None);
	DOREPLIFETIME_CONDITION(AEscapeToWin, bStartBoss, COND_None);

	DOREPLIFETIME_CONDITION_NOTIFY(AEscapeToWin, currentUIText, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEscapeToWin, currentUIColor, COND_None, REPNOTIFY_Always);

}