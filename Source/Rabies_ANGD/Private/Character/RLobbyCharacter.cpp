// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RLobbyCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"


ARLobbyCharacter::ARLobbyCharacter()
{
	OverheadPlayerSpot = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadPlayerSpot"));
	OverheadPlayerSpot->SetupAttachment(RootComponent);
	bReplicates = true;

}

void ARLobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ThisClass, bIsReady);
	DOREPLIFETIME(ThisClass, PlayerName);
}

void ARLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	this->CharacterOverhead = Cast<UOverheadPlayerSpot>(this->OverheadPlayerSpot->GetUserWidgetObject());

	if (IsValid(OverheadPlayerSpot)) {
		this->CharacterOverhead->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ARLobbyCharacter::Multi_SetReadyStatus_Implementation(bool InbIsReady)
{
	this->bIsReady = InbIsReady;

	if (IsValid(this->CharacterOverhead)) {
		this->CharacterOverhead->UpdateReadyStatus(this->bIsReady);
		this->CharacterOverhead->SetVisibility(ESlateVisibility::Visible);
	}
	else {

		FTimerHandle MemberTimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Multi_SetReadyStatus"), InbIsReady);

		GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, TimerDel, 0.03f, false);
	}
}

void ARLobbyCharacter::Multi_SetPlayerName_Implementation(const FString& InPlayerName)
{
	this->PlayerName = InPlayerName;

	if (this->CharacterOverhead != nullptr)
		this->CharacterOverhead->UpdatePlayerName(this->PlayerName);
	else
	{
		FTimerHandle MemberTimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Multi_SetPlayerName"), InPlayerName);

		GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, TimerDel, 0.03f, false);
	}
}

void ARLobbyCharacter::Multi_SetIconAndColorOverheadWidget_Implementation(bool bIsHidden, const FString& InPlayerNameColor)
{

	if (IsValid(this->CharacterOverhead))
	{
		this->CharacterOverhead->SetPlayerNameColor(InPlayerNameColor);
		this->CharacterOverhead->SetReadyStatusVisibility(bIsHidden);
	}
}

void ARLobbyCharacter::Multi_PlayStartLevelMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
		AnimInstance->Montage_Play(this->StartLevelMontage);
}

