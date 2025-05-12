// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniRanged.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniRanged : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ToniRanged();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UFUNCTION()
	void Fire();

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	const FGameplayAbilityActorInfo* actorInfo;

	UPROPERTY()
	FGameplayAbilityActivationInfo activationInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ExplosionParticle;

	UFUNCTION()
	void ExplosionHehe(FVector HitPos);

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;

	UPROPERTY()
	FGameplayAbilitySpecHandle cooldownHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* RightShootingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* LeftShootingMontage;


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RangedDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> ExplosionDamage;

	FDelegateHandle ClientHitScanHandle;

	UPROPERTY()
	class ARPlayerBase* Player;
};
