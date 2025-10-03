// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_TexMelee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_TexMelee : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_TexMelee();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void HandleCombo(FGameplayEventData Payload);

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

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TSubclassOf<class UGameplayEffect> SpecialCooldownClass;

	/*		Extra Audio		*/

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FGameplayTag AudioCueTagAir;

	void TriggerAudioCueAir();
};
