// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_MiniLockSpecial.generated.h"

/**
 * 
 */
UCLASS()
class UGA_MiniLockSpecial : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_MiniLockSpecial();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* StartMontage;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* EndMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> SpecialDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> FireEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ExplosionParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundCue* ExplosionAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundAttenuation* ExplosionSoundAttenuationSettings;

	UFUNCTION()
	void CheckGround();

	UFUNCTION()
	void DoJump();

	FTimerHandle groundCheckHandle;
	FTimerHandle JumpHandle;

	UPROPERTY()
	class ARPlayerBase* Player;

	FTimerHandle GroundCheckTimer;

};
