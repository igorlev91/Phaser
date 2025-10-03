// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniUltimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ToniUltimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* EndingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* StartupMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> UltimateDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Slowdown;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilityStartSoundEfx;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilityMemeStartSoundEfx;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilityActivateSoundEfx;


	FActiveGameplayEffectHandle SlowDownEffectHandle;

	FActiveGameplayEffectHandle SpeedupEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Speedup;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ToniSpinParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TSubclassOf<class UGameplayEffect> UltimateCooldownClass;

	UFUNCTION()
	void RecieveAttack(FGameplayEventData Payload);

	UFUNCTION()
	void ReleaseAttack(FGameplayEventData Payload);

	UFUNCTION()
	void TryAttack(float timer);

	UFUNCTION()
	void TrySwpin();

	FTimerHandle spinhandle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ToniHitParticle;

	FTimerHandle StopUltimateTimer;
	FTimerHandle StopUltimateMode;

	void LaunchAttack(class AREnemyBase* hittingEnemy, int timer);

	void FinishTimerUltimate();

	void FinishUltimateMode();

	void FinishUltimate(FGameplayEventData Payload);

	UPROPERTY()
	UMaterialInstanceDynamic* UltimateModeMat;

	UFUNCTION()
	void UltimateLerp(float value, float desiredValue, float times);

	FTimerHandle UltimateModeTimer;

	FTimerHandle HoldTimer;

	class APostProcessVolume* PostProcessVolume;
};
