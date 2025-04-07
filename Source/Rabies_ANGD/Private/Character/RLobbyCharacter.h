// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RCharacterBase.h"
#include "InputActionValue.h"
#include "Components/WidgetComponent.h"
#include "../Widgets/Lobby/OverheadPlayerSpot.h"
#include "Animation/AnimMontage.h"
#include "RLobbyCharacter.generated.h"


UCLASS()
class ARLobbyCharacter : public ARCharacterBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

public:

	ARLobbyCharacter();

	UPROPERTY(Replicated)
	bool bIsReady;

	UPROPERTY(Replicated)
	FString PlayerName;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadPlayerSpot;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlayerName(const FString& InPlayerName);
	void Multi_SetPlayerName_Implementation(const FString& InPlayerName);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_SetReadyStatus(bool InbIsReady);
	void Multi_SetReadyStatus_Implementation(bool InbIsReady);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_SetIconAndColorOverheadWidget(bool bIsHidden, const FString& InPlayerNameColor);
	void Multi_SetIconAndColorOverheadWidget_Implementation(bool bIsHidden, const FString& InPlayerNameColor);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_PlayStartLevelMontage();
	void Multi_PlayStartLevelMontage_Implementation();

private:
	UOverheadPlayerSpot* CharacterOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim Montage", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* StartLevelMontage;
};
