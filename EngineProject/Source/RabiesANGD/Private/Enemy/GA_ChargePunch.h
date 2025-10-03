// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ChargePunch.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ChargePunch : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_ChargePunch();

private:

	UPROPERTY()
	class AREnemyBase* Enemy;

	UPROPERTY()
	class AAIController* EnemyAI;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> PunchDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* ChargePunchMontage;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void StopChargePunch();

	UFUNCTION()
	void PunchForward();

	UFUNCTION()
	void Popup();

	FTimerHandle ZoomTimerHandle;
	FTimerHandle PopupTimerHandle;
};
