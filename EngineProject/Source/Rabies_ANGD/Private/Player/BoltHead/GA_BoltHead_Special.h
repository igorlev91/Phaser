// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_BoltHead_Special.generated.h"

/**
 * 
 */
UCLASS()
class UGA_BoltHead_Special : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_BoltHead_Special();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void TargetAquired(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UFUNCTION()
	void DelayEnd();

	FTimerHandle DelayEndHandle;

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

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UPROPERTY()
	class ARPlayerBase* Player;

	FTimerHandle PunchHandle;

	float CurrentHoldDuration;

	void Hold(float timeRemaining);
};
