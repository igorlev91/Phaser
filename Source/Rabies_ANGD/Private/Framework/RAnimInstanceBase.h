// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "RAnimInstanceBase.generated.h"

class ACharacter;
class UCharacterMovementComponent;

class URAbilitySystemComponent;
class URAttributeSet;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class URAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsMoving() const { return Speed != 0; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsNotMoving() const { return Speed == 0; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsOnGround() const { return !bIsJumping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	FRotator GetLookOffset() const { return LookOffset; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool ShouldDoUpperBody() const;

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool ShouldDotBeFlapping() const;

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool GetIsAiming() const { return bIsScoping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetFwdSpeed() const { return FwdSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetRightSpeed() const { return RightSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool GetRangedAttacking() const { return bAttacking && bIsScoping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool GetFlying() const { return bFlying && bIsJumping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool GetTiredFlying() const { return DotFlyStamina <= 0 && bIsJumping; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsMovingInAir() const { return AirSpeed >= 300; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsHoldingJump() const { return !bHoldingJump; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsSpecialAiming() const { return bSpecialAiming; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsUltimateAiming() const { return bUltimateAiming; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	bool IsReviving() const { return bReviving; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetMovementSpeedScale() const { return CurrentMovementSpeedScale / BaseMovementSpeedScale; }

	UFUNCTION(BlueprintCallable, Category = "Animation", meta = (BlueprintThreadSafe))
	float GetFlyingMovementSpeedScale() const { return CurrentFlyingSpeedScale / BaseFlyingSpeedScale; }
private:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* StartMontage;

	// the below functions are the native overrides for each phase
	// Native initialization override point BEGIN PLAY
	virtual void NativeInitializeAnimation() override;

	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant TICK
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	const ACharacter* OwnerCharacter;
	const UCharacterMovementComponent* OwnerMovementComp;

	float BaseMovementSpeedScale;
	float BaseFlyingSpeedScale;
	float CurrentMovementSpeedScale;
	float CurrentFlyingSpeedScale;

	bool bBaseMovementSpeedUpdate;

	float Speed;
	float DotFlyStamina;
	float AirSpeed;
	bool bIsJumping;
	bool bHoldingJump;
	FRotator LookOffset;

	FRotator PrevRot;
	float YawSpeed;

	float FwdSpeed;
	float RightSpeed;

	bool bIsScoping;

	bool bSpecialAiming;
	bool bUltimateAiming;

	bool bAttacking;
	bool bDead;
	bool bFlying;
	bool bTiredFlying;
	bool bReviving;

	void AttackingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void RevivingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void SpecialAimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void UltimateAimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void FlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void TiredFlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void HoldingJumpTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	void DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
};
