// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class UGA_AbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AbilityBase();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UTexture2D* GetIconTexture() const { return IconTexture; }

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class UGameplayModMagnitudeCalculation> CooldownCalculationClass;

protected:
	void SignalDamageStimuliEvent(FGameplayAbilityTargetDataHandle TargetHandle);
	void ExecuteSpawnVFXCue(UParticleSystem* VFXToSpawn, float Size, const FVector& Location);


	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* IconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FGameplayTag AudioCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSubclassOf<UGameplayEffect> DurationAudioEffect;

	FActiveGameplayEffectHandle DurationAudioEffectHandle;

	void TriggerAudioCue();
	void StartDurationAudioEffect();
	void StopDurationAudioEffect();
};
