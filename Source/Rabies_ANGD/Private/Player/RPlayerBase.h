// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RCharacterBase.h"
#include "RPlayerBase.generated.h"

class USpringArmComponent;
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

private:

	UPROPERTY(visibleAnywhere, Category = "View")
	USpringArmComponent* cameraBoom;

	UPROPERTY(visibleAnywhere, Category = "View")
	UCameraComponent* viewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector AimCameraLocalOffset;

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
	UInputAction* basicAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* specialAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ultimateAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityConfirmAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AbilityCancelAction;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& InputValue);

	UFUNCTION()
	void Look(const FInputActionValue& InputValue);

	UFUNCTION()
	void DoBasicAttack();

	UFUNCTION()
	void TryActivateSpecialAttack();

	UFUNCTION()
	void TryActivateUltimateAttack();

	UFUNCTION()
	void ConfirmActionTriggered();

	UFUNCTION()
	void CancelActionTriggered();

	FVector GetMoveFwdDir() const;
	FVector GetMoveRightDir() const;
};
