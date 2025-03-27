// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_RangedGattlingAttack.generated.h"

/**
 * 
 */
UCLASS()
class UGA_RangedGattlingAttack : public UGA_AbilityBase
{
public:
	UGA_RangedGattlingAttack();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void SendInputForHitScan(FGameplayEventData Payload);

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos);

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RangedGattlingDamage;

	UFUNCTION()
	void AbilityInputPressed(float TimeWaited);

	bool bAttackCommitted;

	void SetupWaitInputTask();

	FDelegateHandle ClientHitScanHandle;

	UFUNCTION()
	void StopAttacking(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;
};
