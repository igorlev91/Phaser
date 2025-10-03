// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RabiesGameModeBase.h"
#include "Framework/EOSGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Player/RPlayerBase.h"
#include "EngineUtils.h"

void ARabiesGameModeBase::PostLogin(APlayerController* PlayerController)
{

	Super::PostLogin(PlayerController);
	UE_LOG(LogTemp, Warning, TEXT("##################### Player Logged in"))

	if (IsRunningDedicatedServer())
	{
		UEOSGameInstance* RGameInstance = GetGameInstance<UEOSGameInstance>();
		if (RGameInstance)
		{
			RGameInstance->PlayerJoined(PlayerController);
		}
	}

	APawn* ExistingPawn = PlayerController->GetPawn();
	if (!ExistingPawn)
	{
		// If there's no pawn, you can manually spawn one or find one based on player index / ID
		// Example: Reuse already-spawned pawn in level
		for (TActorIterator<ARPlayerBase> It(GetWorld()); It; ++It)
		{
			ARPlayerBase* PlayerChar = *It;
			if (PlayerChar && PlayerChar->GetOwner() == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Poesseion Logged in"))

				PlayerChar->SetOwner(PlayerController);
				PlayerController->Possess(PlayerChar);
				break;
			}
		}
	}
}

void ARabiesGameModeBase::Logout(AController* PlayerController)
{
	Super::Logout(PlayerController);
	UE_LOG(LogTemp, Warning, TEXT("################# Player Logged out"))
	if (IsRunningDedicatedServer())
	{
		UEOSGameInstance* RGameInstance = GetGameInstance<UEOSGameInstance>();
		if (RGameInstance)
		{
			RGameInstance->PlayerLeft(PlayerController);
		}
	}
}
