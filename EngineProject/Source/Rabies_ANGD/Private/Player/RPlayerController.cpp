// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerController.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Actors/ItemPickup.h"
#include "Framework/EOSActionGameState.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Widgets/LoadingScreenUI.h"
#include "Framework/RItemDataAsset.h"
#include "Enemy/REnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/PauseUI.h"
#include "Widgets/GameplayUI.h"
#include "EngineUtils.h"


void ARPlayerController::TryPossessExistingPawn()
{
	for (TActorIterator<ARPlayerBase> It(GetWorld()); It; ++It)
	{
		ARPlayerBase* PlayerChar = *It;
		if (PlayerChar)
		{
			if (PlayerChar->GetOwner() == this)
			{
				FInputModeGameOnly input;
				GetWorld()->GetFirstPlayerController()->SetInputMode(input);
				UE_LOG(LogTemp, Warning, TEXT("Possessing existing player character: %s"), *PlayerChar->GetName());
				Possess(PlayerChar);
				//SetViewTargetWithBlend(PlayerChar, 0.0f);
				return;
			}
		}
	}
}

float ARPlayerController::GetElaspedTime()
{
	if (GameplayUI)
	{
		return GameplayUI->timeElasped;

	}
	return 600.0f;
}

void ARPlayerController::SetAirComboText(int kills) // always returns null for some god forsaken reason
{
	if (!IsLocalPlayerController() || GameplayUI == nullptr)
	{
		return;
	}

	GameplayUI->SetAirComboText(kills);
}

void ARPlayerController::SetCritComboText(int kills, int timeRemaining)
{
	if (GameplayUI)
	{
		GameplayUI->SetCritComboText(kills, timeRemaining);
	}
}

void ARPlayerController::SetHealingGivenText(int health)
{
	if (GameplayUI)
	{
		GameplayUI->SetHealingGivenText(health);
	}
}

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
		FInputModeGameOnly input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);
		PostPossessionSetup(NewPawn);
	}
}

void ARPlayerController::AddBossEnemy(int level, AREnemyBase* bossEnemy)
{
	ImplimentBossEnemy(level, bossEnemy);
}

void ARPlayerController::TogglePauseMenu(bool state)
{
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
		return;

	if (GameState->PlayerArray.Num() <= 1)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), state);
	}

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(state);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = state;

	if (state)
	{
		PauseUI->AddToViewport();
		FInputModeGameAndUI input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);
	}
	else
	{
		FInputModeGameOnly input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);
		PauseUI->RemoveFromParent();
	}
}

void ARPlayerController::ImplimentBossEnemy_Implementation(int level, AREnemyBase* bossEnemy)
{
	if (GameplayUI)
	{
		GameplayUI->AddEnemyBossHealth(level, bossEnemy);
	}
}

void ARPlayerController::FullyDead_Implementation()
{
	if (PlayerBase)
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_PlayerFullyDies(PlayerBase);
		}
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

	FInputModeGameOnly input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(input);

	CreateGameplayUI();
	CreatePauseUI();

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;

	if (IsLocalPlayerController())
	{
		// Set up the players initial attributes
	}

	AEOSPlayerState* playerState = Cast<AEOSPlayerState>(PlayerState);
	if (playerState == nullptr)
	{
		return;
	}
	playerState->OnLoadNewScene.AddUObject(this, &ARPlayerController::CreateLoadingScreenUI);
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

	GameplayUI = CreateWidget<UGameplayUI>(this, GameplayUIClass);
	if (GameplayUI)
	{
		GameplayUI->AddToViewport();
		GameplayUI->OnToyboxDecision.AddUObject(this, &ARPlayerController::CloseToyBox);
		GameplayUI->OnFullyDead.AddUObject(this, &ARPlayerController::FullyDead);

		FInputModeGameOnly input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);
	}
}

void ARPlayerController::CreatePauseUI()
{
	if (!PlayerBase)
	{
		return;
	}

	if (!PauseUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s,  missing Gameplay UI class, Can't Spawn Gameplay UI"), *GetName());
		return;
	}
	//return true, if this player controller represent a human player sitting infront of this manchine.
	if (!IsLocalPlayerController())
	{
		return;
	}

	PauseUI = CreateWidget<UPauseUI>(this, PauseUIClass);

	if (PauseUI)
	{
		PauseUI->OnShowUIChange.AddUObject(this, &ARPlayerController::ToggleUIState);
		PauseUI->OnSensChange.AddUObject(this, &ARPlayerController::SensChange);
	}
}

void ARPlayerController::ToggleUIState(bool state)
{
	if (GameplayUI)
	{
		if (state)
		{
			GameplayUI->AddToViewport();
		}
		else
		{
			GameplayUI->RemoveFromParent();
		}
	}
}

void ARPlayerController::SensChange(float newSens)
{
	if (PlayerBase)
	{
		PlayerBase->Sensitvitiy = newSens;
	}
}

void ARPlayerController::CreateLoadingScreenUI()
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

	/*UWorld* World = GetWorld();

	if (World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("Facility_CourtyardSnow_EndingScene"));
	}*/

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

void ARPlayerController::ChangeSuperPunchState(bool state, float charge)
{
	if (!IsLocalPlayerController() || GameplayUI == nullptr)
	{
		return;
	}

	GameplayUI->SetChargeSuperPunchState(state, charge);
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

bool ARPlayerController::HasGameplayUI()
{
	return GameplayUI != nullptr;
}

bool ARPlayerController::CashMyLuck()
{
	if (GameplayUI == nullptr)
		return false;

	return GameplayUI->CashMyLuck();
}

void ARPlayerController::GameOver()
{
	if (GameplayUI)
	{
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

		FInputModeGameAndUI input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);

		GameplayUI->GameOver();
	}
}

void ARPlayerController::OpenToyBox(AItemPickup* itemToChange, FString itemSelection)
{
	if (GameplayUI && PlayerBase)
	{
		PlayerBase->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());

		FInputModeGameAndUI input;
		GetWorld()->GetFirstPlayerController()->SetInputMode(input);

		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;

		GameplayUI->OpenToyBox(itemToChange, itemSelection);
	}
}

void ARPlayerController::CloseToyBox(AItemPickup* choosingItem, URItemDataAsset* chosenItem)
{
	if (PlayerBase->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == false)
	{
		PlayerBase->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
	}

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;

	FInputModeGameOnly input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(input);

	FixHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerController::FixInput));

	FTimerHandle newHandle;
	GetWorldTimerManager().SetTimer(newHandle, this, &ARPlayerController::FixInput, 0.1f, false);

	ChangeItem(choosingItem, chosenItem);
}

void ARPlayerController::FixInput()
{
	if (PlayerBase->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == false)
	{
		PlayerBase->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
	}
}

void ARPlayerController::ChangeItem_Implementation(AItemPickup* choosingItem, URItemDataAsset* chosenItem)
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Server_RequestChangeItem(choosingItem, chosenItem);
	}
}

bool ARPlayerController::ManualDeathStatusUpdate(bool state)
{
	if (GameplayUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("manual calling death status updated"));
		GameplayUI->DeadStatusUpdated(state);
	}
	return state;
}

void ARPlayerController::Server_RequestRevive_Implementation(AEOSPlayerState* TargetPlayerState)
{
	if (TargetPlayerState)
	{
		TargetPlayerState->Server_RevivePlayer();
	}
}
