// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OpenLobby.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

void UOpenLobby::InitLobbyEntry(const FName& LobbyName, int EntryLobbyIndex)
{
	LobbyNameText->SetText(FText::FromName(LobbyName));
	LobbyIndex = EntryLobbyIndex;
	FText Text = FText::Format(FText::FromString("{0}/4"), FText::AsNumber(EntryLobbyIndex + 1));
	LobbyPlayerLimitText->SetText(Text);
}

void UOpenLobby::LobbyButtonClicked()
{
	OnLobbyEntrySelected.Broadcast(LobbyIndex);
}

void UOpenLobby::LobbyButtonAudio()
{
	UGameplayStatics::PlaySound2D(this, ClickAudio);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UOpenLobby::LobbyButtonClicked, 0.2f, false);
}

void UOpenLobby::PlayHoverAudio()
{
	UGameplayStatics::PlaySound2D(this, HoverAudio);
}

void UOpenLobby::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyButton->OnClicked.AddDynamic(this, &UOpenLobby::LobbyButtonClicked);
	LobbyButton->OnHovered.AddDynamic(this, &UOpenLobby::PlayHoverAudio);
}
