// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniPassive.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniPassive : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ToniPassive();

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DoubleJumpAnim;

	UFUNCTION()
	void ApplyJump(/*FGameplayEventData Payload*/);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void ShootBullets(/*FGameplayEventData Payload*/);

	UFUNCTION()
	void FireAttack(FVector start, FVector direction);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ToniMeleeParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;

	UPROPERTY()
	class ARPlayerBase* Player;

	UFUNCTION()
	void DoubleJumpProcess();

	FTimerHandle ProcessDoubleJumpHandle;
};
