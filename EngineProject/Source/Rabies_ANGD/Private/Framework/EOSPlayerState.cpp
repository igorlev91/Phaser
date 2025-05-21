// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RPlayerBase.h"
#include "Framework/EOSGameState.h"
#include "Player/RPlayerController.h"
#include "Framework/RCharacterDefination.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/ReviveUI.h"
#include "Widgets/HealthBar.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/RSaveGame.h"
#include "Framework/EOSGameInstance.h"


void AEOSPlayerState::OnRep_PickedCharacter()
{
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

void AEOSPlayerState::OnRep_HoveredCharacterIndex()
{
	OnHoveredCharacterIndexReplicated.Broadcast(HoveredCharacterIndex);
}

URCharacterDefination* AEOSPlayerState::GetCharacterDefination() const
{
	return PickedCharacter;
}

void AEOSPlayerState::SeOwnsSettings_Implementation(bool state)
{
	bOwnsSettings = state;
}

bool AEOSPlayerState::SeOwnsSettings_Validate(bool state)
{
	return true;
}

void AEOSPlayerState::SetSpareParts_Implementation(bool state)
{
	bSpareParts = state;
}

bool AEOSPlayerState::SetSpareParts_Validate(bool state)
{
	return true;
}

void AEOSPlayerState::SetToyBox_Implementation(bool state)
{
	bToyBox = state;
	UE_LOG(LogTemp, Warning, TEXT("Setting ToyBox to: %s"), bToyBox ? TEXT("true") : TEXT("false"));
}

bool AEOSPlayerState::SetToyBox_Validate(bool state)
{
	return true;
}

void AEOSPlayerState::SetSettings_Implementation(ESettingsType settings)
{
	GameSettings = settings;
}

bool AEOSPlayerState::SetSettings_Validate(ESettingsType settings)
{
	return true;
}

void AEOSPlayerState::Server_IssueCharacterPick_Implementation(URCharacterDefination* newPickedCharacterDefination)
{
	if (newPickedCharacterDefination == PickedCharacter)
		return;

	AEOSGameState* gameState = Cast<AEOSGameState>(UGameplayStatics::GetGameState(this));
	if (!gameState)
		return;

	gameState->UpdateCharacterSelection(newPickedCharacterDefination, PickedCharacter, GetPlayerName());
	PickedCharacter = newPickedCharacterDefination;
	OnPickedCharacterReplicated.Broadcast(PickedCharacter);
}

bool AEOSPlayerState::Server_IssueCharacterPick_Validate(URCharacterDefination* newPickedCharacterDefination)
{
	return true;
}

void AEOSPlayerState::Server_ChangeHoveredCharacterPick_Implementation()
{
	HoveredCharacterIndex++;

	if (HoveredCharacterIndex == 4)
		HoveredCharacterIndex = 0;

	OnHoveredCharacterIndexReplicated.Broadcast(HoveredCharacterIndex);
}

bool AEOSPlayerState::Server_ChangeHoveredCharacterPick_Validate()
{
	return true;
}

void AEOSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Copied Updated Character"));
	AEOSPlayerState* NewPlayerState = Cast<AEOSPlayerState>(PlayerState);
	NewPlayerState->PickedCharacter = PickedCharacter;
	NewPlayerState->bOwnsSettings = bOwnsSettings;
	NewPlayerState->bToyBox = bToyBox;
	NewPlayerState->bSpareParts = bSpareParts;
	NewPlayerState->GameSettings = GameSettings;
}

AEOSPlayerState::AEOSPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;

}

void AEOSPlayerState::Client_Load_Implementation()
{
	OnLoadNewScene.Broadcast();

	if (UGameplayStatics::DoesSaveGameExist(TEXT("RabiesSaveData"), 0))
	{
		USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame && PickedCharacter)
		{
			if(PickedCharacter->CharacterName == "Chester")
				LoadedGame->bWonAsChester = true;

			if (PickedCharacter->CharacterName == "Toni")
				LoadedGame->bWonAsToni = true;

			if (PickedCharacter->CharacterName == "Tex")
				LoadedGame->bWonAsTex = true;

			if (PickedCharacter->CharacterName == "Dot")
				LoadedGame->bWonAsDot = true;

			if (PickedCharacter->CharacterName == "BoltHead")
				LoadedGame->bWonAsBoltHead = true;

			UGameplayStatics::SaveGameToSlot(LoadedGame, TEXT("RabiesSaveData"), 0);
		}
	}
	else
	{
		URSaveGame* NewSave = Cast<URSaveGame>(UGameplayStatics::CreateSaveGameObject(URSaveGame::StaticClass()));
		if (NewSave && PickedCharacter)
		{
			if (PickedCharacter->CharacterName == "Chester")
				NewSave->bWonAsChester = true;

			if (PickedCharacter->CharacterName == "Toni")
				NewSave->bWonAsToni = true;

			if (PickedCharacter->CharacterName == "Tex")
				NewSave->bWonAsTex = true;

			if (PickedCharacter->CharacterName == "Dot")
				NewSave->bWonAsDot = true;

			if (PickedCharacter->CharacterName == "BoltHead")
				NewSave->bWonAsBoltHead = true;

			UGameplayStatics::SaveGameToSlot(NewSave, TEXT("RabiesSaveData"), 0);
		}
	}
}

bool AEOSPlayerState::Client_Load_Validate()
{
	return true;
}


void AEOSPlayerState::Client_DelayLoad()
{
	OnLoadNewScene.Broadcast();
}



void AEOSPlayerState::Server_RevivePlayer_Implementation()
{
	//if (Player == nullptr)
		//return;

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetReviveTag(), eventData);

	Player->SetPlayerReviveState(false);

	Player->PlayVoiceLine(Player->ReviveSound, 100);
}


void AEOSPlayerState::Server_OnPossessPlayer_Implementation(ARPlayerBase* myPlayer)
{
	Player = myPlayer;
}

bool AEOSPlayerState::Server_OnPossessPlayer_Validate(ARPlayerBase* myPlayer)
{
	return true;
}

void AEOSPlayerState::Server_UpdateHitscanRotator_Implementation(FRotator newRot, FVector newLocation)
{
	//On server
	hitscanLocation = newLocation;
	hitscanRotation = newRot;
}

bool AEOSPlayerState::Server_UpdateHitscanRotator_Validate(FRotator newRot, FVector newLocation)
{
	return true;
}

void AEOSPlayerState::Server_UpdateCameraShake_Implementation(FVector newLoc)
{
	cameraShakeLocation = newLoc;

	if (Player)
	{
		Player->shakePivot->SetRelativeLocation(cameraShakeLocation);
	}
	//FString VectorString = cameraShakeLocation.ToString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MyVector: %s"), *VectorString));
}

bool AEOSPlayerState::Server_UpdateCameraShake_Validate(FVector newLoc)
{
	return true;
}

void AEOSPlayerState::Server_UpdateSocketLocations_Implementation(FVector rootAimingLoc, FVector rangedLoc)
{
	//On server
	RootAiming_SocketLocation = rootAimingLoc;
	Ranged_SocketLocation = rangedLoc;
}

bool AEOSPlayerState::Server_UpdateSocketLocations_Validate(FVector rootAimingLoc, FVector rangedLoc)
{
	return true;
}

void AEOSPlayerState::Server_UpdateAlternateAiming_Implementation(FVector rangedLoc)
{
	AltRanged_SocketLocation = rangedLoc;
}

bool AEOSPlayerState::Server_UpdateAlternateAiming_Validate(FVector rangedLoc)
{
	return true;
}

void AEOSPlayerState::Server_UpdateHoldingUltimate_Implementation(bool state)
{
	bHoldingUltimate = state;
}

bool AEOSPlayerState::Server_UpdateHoldingUltimate_Validate(bool state)
{
	return true;
}


void AEOSPlayerState::Server_CreateBossHealth_Implementation(int level, class AREnemyBase* enemy)
{
	if (HasAuthority())
	{
		if (Player == nullptr)
			return;

		if (Player->playerController == nullptr)
			return;

		Player->playerController->AddBossEnemy(level, enemy);
	}
}

void AEOSPlayerState::SetFullyDead(bool state, ARPlayerBase* player)
{
	if (Player == nullptr)
		Player = player;

	bFullyDead = state;
}

ARPlayerBase* AEOSPlayerState::GetPlayer()
{
	return Player;
}

void AEOSPlayerState::OnRep_HitScanLocation()
{
	if (Player == nullptr)
		return;

	Player->viewPivot->SetRelativeLocation(hitscanLocation);
}

void AEOSPlayerState::OnRep_CameraShake()
{
	if (Player == nullptr)
		return;

	FString VectorString = cameraShakeLocation.ToString();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MyVector: %s"), *VectorString));
	Player->shakePivot->SetRelativeLocation(cameraShakeLocation);
}

void AEOSPlayerState::OnRep_PlayerVelocity()
{
	if(Player != nullptr)
		Player->PlayerVelocity = playerVelocity;
}


void AEOSPlayerState::Server_ProcessDotFly_Implementation(ARPlayerBase* player)
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(player, URAbilityGenericTags::GetApplyGravityJump(), eventData);
	//UE_LOG(LogTemp, Error, TEXT("%s Gravity jump implementaiton"), *GetName());
}

bool AEOSPlayerState::Server_ProcessDotFly_Validate(ARPlayerBase* player)
{
	return true;
}

void AEOSPlayerState::Server_ProcessDotFlyingStamina_Implementation(float newValue)
{
	if (Player == nullptr)
		return;

	dotFlyStamina = newValue;
	Player->DotFlyStamina = dotFlyStamina;
}

bool AEOSPlayerState::Server_ProcessDotFlyingStamina_Validate(float newValue)
{
	return true;
}

void AEOSPlayerState::OnRep_DotFlyStamina()
{
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Getting player"), *GetName());
		Player = Cast<ARPlayerBase>(GetPawn());
		return;
	}

	Player->DotFlyStamina = dotFlyStamina;
}

void AEOSPlayerState::Server_UpdatePlayerVelocity_Implementation(FVector velocity)
{
	if (Player == nullptr)
		return;

	playerVelocity = velocity;
	Player->PlayerVelocity = playerVelocity;
}

bool AEOSPlayerState::Server_UpdatePlayerVelocity_Validate(FVector velocity)
{
	return true;
}


void AEOSPlayerState::OnRep_FullyDead()
{
	if (Player == nullptr)
		return;

	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetFullyDeadTag());

	Player->ItemPickupCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GroundCheckComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Player->ItemPickupCollider->SetGenerateOverlapEvents(false);
	Player->GroundCheckComp->SetGenerateOverlapEvents(false);

	Player->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	Player->GetMesh()->SetVisibility(false, true); // false = not visible, true = propagate to children
	Player->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Player->GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	Player->GetMesh()->SetGenerateOverlapEvents(false);
	Player->GetMesh()->SetSimulatePhysics(false);
	Player->GetMesh()->SetEnableGravity(false);

	Player->SetPlayerReviveState(false);

	if (Player->ReviveUI)
	{
		Player->ReviveUI->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Player->HealthBar)
	{
		Player->HealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AEOSPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Player, COND_None);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, PickedCharacter, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, HoveredCharacterIndex, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, playerVelocity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, dotFlyStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, hitscanRotation, COND_None);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, cameraShakeLocation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, hitscanLocation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AEOSPlayerState, bFullyDead, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, Ranged_SocketLocation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, AltRanged_SocketLocation, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, RootAiming_SocketLocation, COND_None);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, bHoldingUltimate, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, bToyBox, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, bSpareParts, COND_None);
	DOREPLIFETIME_CONDITION(AEOSPlayerState, GameSettings, COND_None);

	DOREPLIFETIME_CONDITION(AEOSPlayerState, bOwnsSettings, COND_None);
}