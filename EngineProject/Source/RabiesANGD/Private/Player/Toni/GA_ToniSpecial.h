// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()
public:
	UGA_ToniSpecial();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* PreparingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<TSubclassOf<class UGameplayEffect>> AttackDamages;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* BulletCasing;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ToniSpecialParticle;

	UFUNCTION()
	void LeftGun(FGameplayEventData Payload);

	UFUNCTION()
	void RightGun(FGameplayEventData Payload);

	UPROPERTY()
	float directionRotLeft;

	UPROPERTY()
	float directionRotRight;


	UFUNCTION()
	void FireAttack(FVector start, FVector direction);

	UPROPERTY()
	class ARPlayerBase* Player;

	TArray<class ARTargetActor_ToniSpecial*> ToniAbilityHints;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_ToniSpecial> targetActorClass;

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;
};
