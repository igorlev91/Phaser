// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotSpecial();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_DotSpecial> targetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Anim;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class ARDot_SpecialProj> DotProjectile;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;
};
