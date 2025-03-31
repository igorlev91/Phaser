// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RCharacterBase.h"
#include "GameplayEffectTypes.h"
#include "RPlayerBase.generated.h"

class USceneComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

DECLARE_MULTICAST_DELEGATE(FOnPlayerInteraction);
/**
 * 
 */
UCLASS()
class ARPlayerBase : public ARCharacterBase
{
	GENERATED_BODY()
	
public:
	ARPlayerBase();

public:

	FOnPlayerInteraction PlayerInteraction;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	bool bRangedAttacking;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class ARPlayerController* playerController;

	UPROPERTY()
	FVector2D MoveInput;

	UFUNCTION()
	void SetRabiesPlayerController(class ARPlayerController* newController);

	UFUNCTION()
	void SetPlayerState();

	UFUNCTION()
	void SetInteractionChest(class AItemChest* chest);

	UFUNCTION()
	void SetInteractionWin(class AEscapeToWin* winPoint);

	UFUNCTION()
	void SetItemPickup(class AItemPickup* itemPickup, class URItemDataAsset* itemAsset);

	UFUNCTION()
	AEOSPlayerState* GetPlayerBaseState();

	UPROPERTY()
	TArray<AActor*> nearbyFaintedActors;

	class AEscapeToWin* escapeToWin;
private:

	UPROPERTY(VisibleAnywhere, Category = "PlayerDetail")
	class USphereComponent* ItemPickupCollider;

	class AItemChest* interactionChest;


	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);

public:

	UFUNCTION(BlueprintCallable, Category = "PlayerDetail")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "PlayerDetail")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bInRangeToRevive;

	float DotFlyStamina = 1.0f;

	UPROPERTY()
	FVector PlayerVelocity;
	
	UPROPERTY(VisibleAnywhere, Category = "View")
	USceneComponent* viewPivot;
private:

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* ReviveUIWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UReviveUI> ReviveUIClass;

	UPROPERTY()
	class UReviveUI* ReviveUI;

	UPROPERTY(visibleAnywhere, Category = "View")
	UCameraComponent* viewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector AimCameraLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector AbilityCameraLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector UltimateCameraLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector DefaultCameraLocal;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	float AimCameraLerpingSpeed = 5;

	virtual void PawnClientRestart() override;

    //////////////////////////////
	/*         Inputs           */
    //////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* inputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* moveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* lookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* jumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* scopeInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* scrollInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* basicAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* specialAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ultimateAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* QuitOutAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PausingInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LoadDebugInputAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& InputValue);

	UFUNCTION()
	void RotatePlayer(float DeltaTime);

	UFUNCTION()
	void Look(const FInputActionValue& InputValue);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void ReleaseJump();

	UFUNCTION()
	void QuitOut();

	UFUNCTION()
	void DoBasicAttack();

	UFUNCTION()
	void StopBasicAttack();


	UFUNCTION()
	void EnableScoping();

	UFUNCTION()
	void DisableScoping();

	UFUNCTION()
	void Scroll(const FInputActionValue& InputActionVal);

	UFUNCTION()
	void TryActivateSpecialAttack();

	UFUNCTION()
	void FinishSpecialAttack();

	UFUNCTION()
	void TryActivateUltimateAttack();
	
	UFUNCTION()
	void FinishUltimateAttack();


	UFUNCTION()
	void Interact();

	UFUNCTION()
	void Pause();

	UFUNCTION()
	void LoadDebug();

	FVector GetMoveFwdDir() const;
	FVector GetMoveRightDir() const;

	virtual void ScopingTagChanged(bool bNewIsAiming) override;


	void LerpCameraToLocalOffset(const FVector& LocalOffset);
	void TickCameraLocalOffset(FVector Goal);
	FTimerHandle CameraLerpHandle;

	float cameraClampMin;
	float cameraClampMax;
	bool bIsScoping;

public:

	/////////////////////////////////
	/*          Pause	           */
	////////////////////////////////

	bool isPaused;

	void SetPausetoFalse();

	/////////////////////////////////
	/*          Anim	           */
	////////////////////////////////

	/////////////////////////////////
	/*          Passives           */
	////////////////////////////////

private:

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bInstantJump;

	/////////////////////////////////
	/*          Online             */
	////////////////////////////////

	UPROPERTY()
	class AEOSPlayerState* EOSPlayerState;

	UFUNCTION(Server, Reliable)
	void ServerRequestInteraction(class AItemChest* Chest, class AEscapeToWin* winPoint);

	UFUNCTION(Server, Reliable)
	void ServerRequestPickupItem(class AItemPickup* itemPickup, class URItemDataAsset* itemAsset);

public:

	UFUNCTION(NetMulticast, Unreliable)
	void ServerSetPlayerReviveState(bool state);

	UFUNCTION(Client, Reliable)
	void AddNewItem(class URItemDataAsset* newItemAsset);

	/////////////////////////////////
	/*           Audio             */
	////////////////////////////////

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* JumpAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* LandAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* WalkAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* PickupAudio;

	void PlayPickupAudio();

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* GroundCheckComp;

	UFUNCTION()
	void GroundCheckCompOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
