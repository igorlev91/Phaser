// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotFlying.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotFlying : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotFlying();

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void TryPickUpTeammates();

	bool bHasTeammatePickedup;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> FlyingSpeedClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityFallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AirComboReset;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityJumpClass;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TakeOffMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* HardLandingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TiredFall;

	bool inFall;

	UFUNCTION()
	void StopFlying();

	void ProcessFlying();

	UFUNCTION()
	void StopTakeOff(FGameplayEventData Payload);

	FTimerHandle TakeOffHandle;
	FTimerHandle GravityHandle;
	FTimerHandle SlowFallHandle;

	float CurrentHoldDuration;
	float CurrentGravityDuration;
	bool bFlying;

	void Hold(float timeRemaining);
	void GravityJump(float timeRemaining);

	UFUNCTION()
	void ApplyGravityJump(FGameplayEventData Payload);

	UFUNCTION()
	void RemoveGravityJump();

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	class ARPlayerBase* YoinkedPlayer;

	FActiveGameplayEffectHandle FlyingSpeedEffectHandle;
	FActiveGameplayEffectHandle GravityFallEffectHandle;
	FActiveGameplayEffectHandle GravityJumpEffectHandle;
};
