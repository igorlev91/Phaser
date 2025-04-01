// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerController.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSPlayerState.h"

#include "Framework/RItemDataAsset.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Widgets/GameplayUI.h"

void ARPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PostPossessionSetup(NewPawn);
	if (PlayerBase)
	{
		//PlayerCharacter->InitAbilities();
	}
}

void ARPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (!HasAuthority())
	{
		PostPossessionSetup(NewPawn);
	}
}

void ARPlayerController::AddBossEnemy(int level, AREnemyBase* bossEnemy)
{
	ImplimentBossEnemy(level, bossEnemy);
}

void ARPlayerController::ImplimentBossEnemy_Implementation(int level, AREnemyBase* bossEnemy)
{
	if (GameplayUI)
	{
		GameplayUI->AddEnemyBossHealth(level, bossEnemy);
	}
}

void ARPlayerController::PostPossessionSetup(APawn* NewPawn)
{
	PlayerBase = Cast<ARPlayerBase>(NewPawn);
	if (!PlayerBase)
	{
		UE_LOG(LogTemp, Error, TEXT("Needs CPlayerCharacter as Pawn"));
		return;
	}

	PlayerBase->SetRabiesPlayerController(this);
	PlayerBase->SetPlayerState();
	// Set up the players ability system

	CreateGameplayUI();
	if (IsLocalPlayerController())
	{
		// Set up the players initial attributes
	}
}

void ARPlayerController::CreateGameplayUI()
{
	if (!PlayerBase)
	{
		return;
	}

	if (!GameplayUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}
	//return true, if this player controller represent a human player sitting infront of this manchine.
	if (!IsLocalPlayerController())
	{
		return;
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;

	FInputModeGameOnly game;
	GetWorld()->GetFirstPlayerController()->SetInputMode(game);

	GameplayUI = CreateWidget<UGameplayUI>(this, GameplayUIClass);
	if (GameplayUI)
	{
		GameplayUI->AddToViewport();
	}
}

void ARPlayerController::ChangeCrosshairState(bool state)
{
	if (GameplayUI)
	{
		GameplayUI->SetCrosshairState(state);
	}

}

void ARPlayerController::ChangeTakeOffState(bool state, float charge)
{
	if (!IsLocalPlayerController() || GameplayUI == nullptr)
	{
		return;
	}

	GameplayUI->SetTakeOffBarState(state, charge);
}

void ARPlayerController::ChangeRevivalState(bool state, float charge)
{
	if (!IsLocalPlayerController() || GameplayUI == nullptr)
	{
		return;
	}

	GameplayUI->SetRevivalBarState(state, charge);
}

void ARPlayerController::AddNewItemToUI(URItemDataAsset* newItemAsset)
{
	if (!IsLocalPlayerController() || GameplayUI == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("failed item"));
		return;
	}
	GameplayUI->AddItem(newItemAsset);

}

bool ARPlayerController::CashMyLuck()
{
	if (GameplayUI == nullptr)
		return false;

	return GameplayUI->CashMyLuck();
}

void ARPlayerController::Server_RequestRevive_Implementation(AEOSPlayerState* TargetPlayerState)
{
	if (TargetPlayerState)
	{
		TargetPlayerState->Server_RevivePlayer();
	}
}
