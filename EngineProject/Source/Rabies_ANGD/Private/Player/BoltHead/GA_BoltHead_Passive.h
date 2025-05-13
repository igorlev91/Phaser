// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_BoltHead_Passive.generated.h"

/**
 * 
 */
UCLASS()
class UGA_BoltHead_Passive : public UGA_AbilityBase
{
	GENERATED_BODY()
	
private:
	UGA_BoltHead_Passive();

private:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> AbilitySoundEfx;

	UPROPERTY()
	class ARPlayerBase* Player;

	UPROPERTY()
	class URBoltHead_Head* BoltHead;

	FTimerHandle DeathCheckerHandle;
	FTimerHandle FinishedHandle;

	void Checker();

	void WeeWoo(class ARPlayerBase* damagedPlayer);

	bool bBusy;
};
