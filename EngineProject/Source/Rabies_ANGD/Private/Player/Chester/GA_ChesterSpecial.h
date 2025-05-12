// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ChesterSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ChesterSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_ChesterSpecial();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void StopRunning(FGameplayEventData Payload);

	UFUNCTION()
	void RunForward();

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;


	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_ChesterSpecial> targetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RunningClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> SlowdownClass;

	FActiveGameplayEffectHandle SlowDownEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingParticle;

	FTimerHandle ZoomTimerHandle;

	class ARTargetActor_ChesterSpecial* ChesterAbilityHint;

	bool bStopRunning;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;

	/*		Extra Audio		*/

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FGameplayTag AudioCueTagAir;
};
