// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_BoltHead_Ranged.generated.h"

/**
 * 
 */
UCLASS()
class UGA_BoltHead_Ranged : public UGA_AbilityBase
{
	GENERATED_BODY()


public:
	UGA_BoltHead_Ranged();
	
private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void Fire(FGameplayEventData Payload);

	UFUNCTION()
	void ForceEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* ShootingMontage;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> RangedDamage;

	FTimerHandle AttackTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class ARBoltHead_RangedProj> BoltHeadProjectile;

	UPROPERTY()
	class ARPlayerBase* Player;
};
