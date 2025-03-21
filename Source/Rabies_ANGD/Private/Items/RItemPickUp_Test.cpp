// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RItemPickUp_Test.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ARItemPickUp_Test::GiveUpgrade()
{
	ARPlayerBase* CurrentPlayer = Cast<ARPlayerBase>(UGameplayStatics::GetPlayerCharacter(this,0));

	if (CurrentPlayer)
	{
		UCharacterMovementComponent* MovementComp = CurrentPlayer->GetCharacterMovement();

		if (MovementComp)
		{
			MovementComp->MaxWalkSpeed += AddedSpeed;
		}

		UE_LOG(LogTemp, Error, TEXT("Change Player Speed."));
	}
}
