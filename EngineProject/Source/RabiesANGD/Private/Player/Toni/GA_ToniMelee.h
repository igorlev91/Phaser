// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniMelee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniMelee : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ToniMelee();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void LeftGun(FGameplayEventData Payload);

	UFUNCTION()
	void RightGun(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ToniMeleeParticle;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void FireAttack(FVector start, FVector direction);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> Speedup;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	/*		Extra Audio		*/
};
