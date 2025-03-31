// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_Scoping.generated.h"

/**
 *
 */
UCLASS()
class UGA_Scoping : public UGA_AbilityBase
{
	
public:
	UGA_Scoping();

private:
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> ScopeSlowdownClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bSniperMode;

	UFUNCTION()
	void StopScoping(FGameplayEventData Payload);

	FActiveGameplayEffectHandle ScopeSlowDownEffectHandle;

	FTimerHandle ScopingHandle;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY(VisibleAnywhere, Category = "Damage")
	TArray<class ARCharacterBase*> VisibleCharacters;

	UFUNCTION()
	void HoldingScope();
};
