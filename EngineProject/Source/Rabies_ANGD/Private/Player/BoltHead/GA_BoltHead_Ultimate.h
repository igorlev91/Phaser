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


	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	UMaterialInstanceDynamic* UltimateModeMat;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* RadioSystem;

	UFUNCTION()
	void DoExplosion();

	UFUNCTION()
	void UltimateLerp(float value, float desiredValue, float times);

	FTimerHandle UltimateModeTimer;

	class APostProcessVolume* PostProcessVolume;
};
