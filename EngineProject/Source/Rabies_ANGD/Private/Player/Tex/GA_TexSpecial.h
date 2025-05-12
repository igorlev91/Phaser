// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_TexSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_TexSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()
public:
	UGA_TexSpecial();

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

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> CooldownReduction;

	UFUNCTION()
	void SendOffAttack(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ARTargetActor_TexSpecial> targetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class ARTex_SpecialProj> TexProjectile;

	TArray<ARTargetActor_TexSpecial*> TexAbilityHints;

	UPROPERTY()
	float directionRot;

	FTimerHandle DisappearKnivesTimer;

	UFUNCTION()
	void DisappearKnives();

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* playTargettingMontageTask;
};
