// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_ToniRanged.generated.h"

/**
 * 
 */
UCLASS()
class UGA_ToniRanged : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_ToniRanged();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UFUNCTION()
	void Fire();

	const FGameplayAbilityActorInfo* actorInfo;

	UPROPERTY()
	FGameplayAbilityActivationInfo activationInfo;

	UPROPERTY()
	FGameplayAbilitySpecHandle cooldownHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* RightShootingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* LeftShootingMontage;


	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> RangedDamage;

	FDelegateHandle ClientHitScanHandle;

	UPROPERTY()
	class ARPlayerBase* Player;
};
