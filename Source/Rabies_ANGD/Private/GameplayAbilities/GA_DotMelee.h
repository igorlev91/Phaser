// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_DotMelee.generated.h"

/**
 * 
 */
UCLASS()
class UGA_DotMelee : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_DotMelee();

private:

	bool bDealDamage = true;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void HandleDamage(FGameplayEventData Payload);

	UFUNCTION()
	void HandleEnemyPush(FGameplayEventData Payload);

	UFUNCTION()
	void DotSuperZoom();

	UFUNCTION()
	void DotFall();

	UFUNCTION()
	void DotRise();

	UFUNCTION()
	void ApplyEffect(float value);

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* MeleeAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* FlyingAttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> MeleePushClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> GravityClass;

	FActiveGameplayEffectHandle GravityFallEffectHandle;

	FTimerHandle ZoomTimerHandle;
	FTimerHandle FallTimerHandle;
	FTimerHandle RiseTimerHandle;

	/*		Extra Audio		*/

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FGameplayTag AudioCueTagAir;

	void TriggerAudioCueAir();

};
