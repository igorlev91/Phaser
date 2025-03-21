// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RCharacterBase.h"
#include "RPlayerBase.generated.h"

class USceneComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class ARPlayerController* playerController;

	UPROPERTY(VisibleAnywhere, Category = "View")
	USceneComponent* viewPivot;

	UPROPERTY(visibleAnywhere, Category = "View")
	UCameraComponent* viewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector AimCameraLocalOffset;

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
	UInputAction* AbilityConfirmAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityCancelAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* QuitOutAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PausingInputAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& InputValue);

	UFUNCTION()
	void Look(const FInputActionValue& InputValue);

	UFUNCTION()
	void QuitOut();

	UFUNCTION()
	void DoBasicAttack();

	UFUNCTION()
	void EnableScoping();

	UFUNCTION()
	void DisableScoping();

	UFUNCTION()
	void Scroll(const FInputActionValue& InputActionVal);

	UFUNCTION()
	void TryActivateSpecialAttack();

	UFUNCTION()
	void TryActivateUltimateAttack();

	UFUNCTION()
	void ConfirmActionTriggered();

	UFUNCTION()
	void CancelActionTriggered();

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void Pause();

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
	/*          Interact           */
	////////////////////////////////

	bool canInteract;

	public:
	void SetInteraction(bool setInteract);

	/////////////////////////////////
	/*          Pause	           */
	////////////////////////////////

	bool isPaused;

	public:
	void SetPausetoFalse();

};
