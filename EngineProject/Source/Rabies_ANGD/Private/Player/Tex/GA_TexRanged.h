// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_TexRanged.generated.h"

/**
 * 
 */
UCLASS()
class UGA_TexRanged : public UGA_AbilityBase
{
public:
	UGA_TexRanged();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);


	UFUNCTION()
	void UltNoise();


	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilityUltimateSoundEfx;

	UFUNCTION()
	void Fire();

	const FGameplayAbilityActorInfo* actorInfo;

	UPROPERTY()
	FGameplayAbilityActivationInfo activationInfo;

	UPROPERTY()
	FGameplayAbilitySpecHandle cooldownHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* ShootingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* UltimateMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingParticle;


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RangedDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> CritShotReset;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AddCritShot;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AddTimeToCritTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> CritRangedDamage;

	FDelegateHandle ClientHitScanHandle;
	FTimerHandle UltimateTimerHandle;

	UPROPERTY()
	class ARPlayerBase* Player;
};
