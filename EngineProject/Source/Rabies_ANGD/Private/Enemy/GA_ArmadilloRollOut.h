// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ArmadilloRollOut.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ArmadilloRollOut : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ArmadilloRollOut();

private:

	UPROPERTY()
	class AREnemyBase* Enemy;

	FName RollingBlackboardKeyName = TEXT("bIsRolling");


	UPROPERTY()
	class AAIController* EnemyAI;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FActiveGameplayEffectHandle SpeedupEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> CollideDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RollingClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Basic_RollIntoBall;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Ball_RollIntoBall;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Basic_RevUp;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Ball_RevUp;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Basic_RollLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Ball_RollLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Basic_RollToDizzy;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Ball_RollToDizzy;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Basic_RollToCollide;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Ball_RollToCollide;

	UPROPERTY()
	class USkeletalMeshComponent* RollerSkeletalMesh;

	UFUNCTION()
	void FinishRollingIntoBall();

	bool bStopRolling;

	UFUNCTION()
	void FinishRevingUp();

	UFUNCTION()
	void RollForward();

	UFUNCTION()
	void ExhaustedRolling();

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playRollIntoBallMontageTask;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playRevUpMontageTask;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playRollLoopMontageTask;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playRollToDizzyMontageTask;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playRollToCollideMontageTask;

	UFUNCTION()
	void FinishAbility();

	UFUNCTION()
	void HitCollide();

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UFUNCTION()
	void HandleRollOutDamage(FGameplayEventData Payload);
};
