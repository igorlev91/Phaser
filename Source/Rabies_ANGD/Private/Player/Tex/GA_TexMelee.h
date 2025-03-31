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

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	/*		Extra Audio		*/

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FGameplayTag AudioCueTagAir;

	void TriggerAudioCueAir();
};
