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

	UFUNCTION(NetMulticast, Reliable)
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

	UFUNCTION()
	void StartToniRevertSpecialCooldown();

	UFUNCTION()
	void RevertSpecialCooldown();

	UFUNCTION()
	void OpenToyBox(class AItemPickup* itemToChange, FString itemSelection);

	UFUNCTION()
	void PlayVoiceLine(TArray<USoundBase*> soundEffects, int activationChance);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> ItemPickupSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> BigStickSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> BagofNailsSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> SprayBottleSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AmmoSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> FriendshipSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> ReviveSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> RevivingFriendSound;

	UPROPERTY()
	float Sensitvitiy;


private:

	UFUNCTION()
	void FocusGame();

	UFUNCTION()
	void CheckGameOver();

	UFUNCTION(Server, Reliable)
	void Server_PlayVoiceLine(USoundBase* soundEffect);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> HurtSounds;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> DeathSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> LuckyChesterSounds;

	void LevelUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> ToniSpecialCooldown;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> ChesterHealEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> ChesterPassiveAdjustment;

	UFUNCTION(Server, Reliable)
	void Server_HandlePing(FVector hitPoint, AActor* hitActor);

	UFUNCTION()
	void SetAllyHealthBars();

	UPROPERTY(EditDefaultsOnly, Category = "Tex Invis")
	UMaterial* TexDefaultMat;  // The base material reference in the editor

	UPROPERTY(EditDefaultsOnly, Category = "Tex Invis")
	UMaterial* TexStealthMat;  // The base material reference in the editor

	UPROPERTY(VisibleAnywhere, Category = "Tex Invis")
	UMaterialInstanceDynamic* DynamicTexMaterialInstance;  // The dynamic material instance

	UFUNCTION()
	void FrameDelayItemPickup(class AItemPickup* newItem);

	class AItemChest* interactionChest;


	UFUNCTION()
	void DeadStatusUpdated(bool bIsDead);

	UFUNCTION()
	void InvisStatusUpdated(bool bIsDead);

	UFUNCTION()
	void CheckChesterHeal();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ChesterHealParticle;

	ARPlayerBase* dotHijack;

public:

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetDotHijack(ARPlayerBase* dot);

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

	UPROPERTY(VisibleAnywhere, Category = "View")
	class USpringArmComponent* viewSpringArm;

	UPROPERTY(VisibleAnywhere, Category = "View")
	USceneComponent* shakePivot;

	UFUNCTION()
	void StartCameraShake(float intensity, float duration);

	UFUNCTION()
	void CameraShake(float intensity, float duration);

	FTimerHandle ScreenShakeLerpHandle;
	FTimerHandle ChesterHealthHandle;
	FTimerHandle GameOverHandle;

	UPROPERTY()
	class ATexKnifesActor* TexKnives;

	UPROPERTY(EditDefaultsOnly, Category = "TexKnives")
	TSubclassOf<class ATexKnifesActor> TexKnivesClass;

	UPROPERTY(EditDefaultsOnly, Category = "TexKnives")
	bool bMakeTexKnives;

	UPROPERTY()
	class AChesterBallActor* ChesterBall;

	UPROPERTY(EditDefaultsOnly, Category = "ChesterBall")
	TSubclassOf<class AChesterBallActor> ChesterBallClass;

	UPROPERTY(EditDefaultsOnly, Category = "ChesterBall")
	bool bMakeChesterBall;

	void DealtDamageCameraShake(class ARCharacterBase* damagedCharacter);
	void RecievedDamage(float damage);

	UPROPERTY()
	class UReviveUI* ReviveUI;

private:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf <class ADamagePopupActor> DamagePopupActorClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* ReviveUIWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UReviveUI> ReviveUIClass;

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
	UInputAction* PingInputAction;

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
	void DoAutomaticAttack();


	UFUNCTION()
	void Ping();

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

	/////////////////////////////////
	/*          Pause	           */
	////////////////////////////////

	bool isPaused;

public:

	void SetPausetoFalse();

	/////////////////////////////////
	/*          Anim	           */
	////////////////////////////////

	/////////////////////////////////
	/*          Passives           */
	////////////////////////////////

	FTimerHandle PickupItemHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bFeelinLucky;

	UFUNCTION()
	bool CashMyLuck();


	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	UAnimMontage* RevivingBuddy;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bInstantJump;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bHasDoubleCheck;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bAlternateAim;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bInstantPassive;

	/////////////////////////////////
	/*          Online             */
	////////////////////////////////

	UPROPERTY()
	class AEOSPlayerState* EOSPlayerState;

	UFUNCTION(Server, Reliable)
	void ServerRequestInteraction(class AItemChest* Chest, class AEscapeToWin* winPoint, bool bLucky);

	UFUNCTION(Server, Reliable)
	void ServerRequestPickupItem(class AItemPickup* itemPickup, class URItemDataAsset* itemAsset);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCameraShake(FVector pos);

public:

	bool bWindingUp;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bMiniLock;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	bool bBolthead;

	UFUNCTION()
	USkeletalMeshComponent* GetItemAttachmentMesh(FName itemName);

	UFUNCTION(NetMulticast, Unreliable)
	void SetPlayerReviveState(bool state);


	UFUNCTION(Server, Unreliable)
	void Multicast_SetHoldingUltimate(bool state);

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

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* GroundCheckComp;

	UPROPERTY(VisibleAnywhere, Category = "PlayerDetail")
	class USphereComponent* ItemPickupCollider;

	void PlayPickupAudio();

	UFUNCTION()
	void GroundCheckCompOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
