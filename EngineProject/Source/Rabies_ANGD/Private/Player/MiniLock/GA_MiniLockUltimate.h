// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_MiniLockUltimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_MiniLockUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_MiniLockUltimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Buff;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY()
	class ARPlayerBase* Player;

	FTimerHandle StopUltimateTimer;
	FTimerHandle StopUltimateMode;

	void FinishTimerUltimate();

	void FinishUltimateMode();

	void FinishUltimate(FGameplayEventData Payload);

	void CastUltimate();

	void CastShockwaveUltimate();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* CastParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* LingerParticle;

	UPROPERTY()
	UMaterialInstanceDynamic* UltimateModeMat;

	UFUNCTION()
	void UltimateLerp(float value, float desiredValue, float times);

	FTimerHandle UltimateModeTimer;

	FTimerHandle HoldTimer;

	class APostProcessVolume* PostProcessVolume;
};
