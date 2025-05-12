// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ChesterMelee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ChesterMelee : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_ChesterMelee();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void HandleCombo(FGameplayEventData Payload);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> MeleeSoundEfx;

	UFUNCTION()
	void TryCommitCombo(FGameplayEventData Payload);

	UFUNCTION()
	void AbilityInputPressed(float timeWaited);

	UFUNCTION()
	void SetupWaitInputTask();

	FName NextComboName;

	bool bComboComitted;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	FActiveGameplayEffectHandle PunchArmorEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> PunchArmor;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> StunEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<class UNiagaraSystem*> HitParticles;

	/*		Extra Audio		*/
};
