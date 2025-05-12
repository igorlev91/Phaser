// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_TexUltimate.generated.h"


class ARCharacterBase;
/**
 * 
 */
UCLASS()
class UGA_TexUltimate : public UGA_AbilityBase
{
	GENERATED_BODY()

public:
	UGA_TexUltimate();

private:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	UAnimMontage* CastingMontage;

	FDelegateHandle ClientHitScanHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> UltimateDamage;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> CritUltimateDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* CritHitParticle;

	FTimerHandle StopTimer;
	FTimerHandle ShootingBounceTimer;

	UPROPERTY()
	TArray<ARCharacterBase*> alreadyHitEnemies;

	bool hasCrit = false;
	FVector start;
	FVector end;

	UFUNCTION()
	void SendBounce();

	UFUNCTION()
	void RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit);

	UPROPERTY()
	class ARPlayerBase* Player;


	void FinishTimerUltimate();

	void FinishUltimate(FGameplayEventData Payload);

	UPROPERTY()
	UMaterialInstanceDynamic* UltimateModeMat;

	UFUNCTION()
	void UltimateLerp(float value, float desiredValue, float times);

	FTimerHandle UltimateModeTimer;

	class APostProcessVolume* PostProcessVolume;
};
