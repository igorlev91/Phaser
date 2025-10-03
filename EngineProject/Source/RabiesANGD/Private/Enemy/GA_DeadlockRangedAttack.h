// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DeadlockRangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DeadlockRangedAttack : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DeadlockRangedAttack();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UFUNCTION()
	void TryCommitRightAttack(FGameplayEventData Payload);

	UFUNCTION()
	void TryCommitLeftAttack(FGameplayEventData Payload);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void StartAiming(FGameplayEventData Payload);

	UFUNCTION()
	void EndAiming(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* AttackAnim;

	FDelegateHandle ClientHitScanHandle;

	class AREnemyBase* Character;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingParticle;

	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";


	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetGunName = "gun";

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ExplosionParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;

	UFUNCTION()
	void ExplosionHehe(FVector HitPos);

};
