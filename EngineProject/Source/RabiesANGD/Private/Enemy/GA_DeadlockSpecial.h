// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DeadlockSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DeadlockSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DeadlockSpecial();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UFUNCTION()
	void ApplyEffect(float value);

	UPROPERTY()
	class AREnemyBase* Enemy;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void TryCommitAttack(FGameplayEventData Payload);

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	class AAIController* EnemyAI;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetBlackboardKeyName = "Target";

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* StartMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* EndMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityClass;

	FActiveGameplayEffectHandle GravityFallEffectHandle;

	void TickLandOnPlayer(float timeRemaining);

	void DoLaunch();

	void EnablePhysicsDelay();

	void LandingExplosion();

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UMaterialInterface* DeadlockDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UMaterialInterface* DefaultDecalMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> FireEffect;

	FTimerHandle LandingLerpHandle;
	FTimerHandle StartWaitHandle;
	FTimerHandle EnablePhysicsLerpHandle;
	FTimerHandle LandingExplosionHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ExplosionParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;
};
