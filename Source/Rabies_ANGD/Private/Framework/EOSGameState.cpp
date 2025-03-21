// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/EOSGameState.h"
#include "Net/UnrealNetwork.h"

void AEOSGameState::SetSessionName(const FName& updatedSessionName)
{
	SessionName = updatedSessionName;
}

void AEOSGameState::OnRep_SessionName()
{
	OnSessionNameReplicated.Broadcast(SessionName);
}

void AEOSGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AEOSGameState, SessionName, COND_None, REPNOTIFY_Always);

}
