// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Clipboard.generated.h"

UCLASS()
class AClipboard : public AActor
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class URCharacterDefination* DefaultCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	FVector ClipboardPosShown;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	FVector ClipboardPosHidden;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	FRotator ClipboardRotShown;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	FRotator ClipboardRotHidden;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class UStaticMeshComponent* ClipboardMesh;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* InteractWidgetComp;

	UPROPERTY()
	class UClipboardInfo* infoWidget;

	FTimerHandle MoveTimerHandle;
	FTimerHandle DelayTimerHandle;

	class URCharacterDefination* CurrentCharacter;

	void MoveClipboard(FVector goal, FRotator rotationGoal);

	void SetClipboardInfo(class URCharacterDefination* Character);

	void FinishNewCharacter();

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* MoveAudio;

public:	
	// Sets default values for this actor's properties
	AClipboard();

	void SetNewCharacter(class URCharacterDefination* Character);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
