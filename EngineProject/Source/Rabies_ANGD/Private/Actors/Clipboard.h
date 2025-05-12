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

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	TSubclassOf<class AAbilityHitBox> HitBoxClass;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class AAbilityHitBox* MeleeHitBox;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class AAbilityHitBox* RangedHitBox;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class AAbilityHitBox* SpecialHitBox;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class AAbilityHitBox* UltimateHitBox;

	UPROPERTY(EditDefaultsOnly, Category = "Clipboard Detail")
	class AAbilityHitBox* PassiveHitBox;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* InteractWidgetComp;

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

	UPROPERTY(VisibleAnywhere, Category = "Clipboard Detail")
	class AActor* MonitorPreview;

	UPROPERTY(VisibleAnywhere, Category = "Clipboard Detail")
	class AActor* RecordingActor;

	UPROPERTY(VisibleAnywhere, Category = "Clipboard Detail")
	class UMeshComponent* ComputerScreen;

	void DelayedShow();

	FTimerHandle DelayShow;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInstance* DefaultMaterialInstance;  // The dynamic material instance

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInstance* StaticMaterialInstance;  // The dynamic material instance

public:	

	UPROPERTY()
	class UClipboardInfo* infoWidget;

	// Sets default values for this actor's properties
	AClipboard();

	void SetNewCharacter(class URCharacterDefination* Character);

	UFUNCTION()
	void OpenMovie(int index);

	UFUNCTION()
	void CloseMovie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
