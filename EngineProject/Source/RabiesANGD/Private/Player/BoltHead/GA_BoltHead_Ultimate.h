// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_BoltHead_Ultimate.generated.h"

/**
 * 
 */
UCLASS()
class UGA_BoltHead_Ultimate : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_BoltHead_Ultimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	float spinAmount;

	FActiveGameplayEffectHandle SpeedupEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Speedup;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	UMaterialInstanceDynamic* UltimateModeMat;

	UPROPERTY()
	USkeletalMeshComponent* TorsoSkeletalMesh;

	UPROPERTY()
	class ARBoltHead_Actor* BoltHead;

	UFUNCTION()
	void DoUltimate(float timeRemaining);

	UFUNCTION()
	void SpinDamageCheck();

	UFUNCTION()
	void HealingSpinCheck();

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> UltimateDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> UltimateHeal;


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* UltimateHealParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BoltHeadSpinParticle;

	UFUNCTION()
	void UltimateLerp(float value, float desiredValue, float times);

	FTimerHandle UltimateModeTimer;

	class APostProcessVolume* PostProcessVolume;
};
