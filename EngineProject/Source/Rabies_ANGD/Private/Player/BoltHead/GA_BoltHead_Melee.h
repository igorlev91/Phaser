// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_BoltHead_Melee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_BoltHead_Melee : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_BoltHead_Melee();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void FireSpark(FGameplayEventData Payload);

	UFUNCTION()
	void FireVFX(FGameplayEventData Payload);

	UFUNCTION()
	void ForceEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* RadioSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* MeleeStunParticle;


	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	/*		Extra Audio		*/
};
