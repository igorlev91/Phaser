// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotUltimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotUltimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	class APostProcessVolume* PostProcessVolume;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void CheckDamage();

	UFUNCTION()
	void StopAttacking();

	FTimerHandle DamageTimer;
	FTimerHandle StopTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> FireUltimateEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontageAir;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontageAir;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> BigAttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityFallClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> MeleePushClass;

	UPROPERTY()
	TSubclassOf<class UGameplayEffect> CurrentDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_DotSpecial> targetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_DotUltimate> targetUltimateActorClass;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UPROPERTY(VisibleAnywhere, Category = "Targetting")
	class ARTargetActor_DotUltimate* TargetActorDotUltimate;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	UFUNCTION()
	void SendOffFinalAttack(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HitEffectLaserBeam;

	float CurrentScale;
	float ScorchSize;

	FDelegateHandle ClientHitScanHandle;

	float CylinderRadius;
	float CylinderHeight;
	float CylinderScale;
	FRotator CylinderRotation;
	FVector CylinderLocation;
};
