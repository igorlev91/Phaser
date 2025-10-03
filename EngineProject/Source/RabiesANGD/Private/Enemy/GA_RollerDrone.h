// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GA_RollerDrone.generated.h"

/**
 * 
 */
UCLASS()
class UGA_RollerDrone : public UGA_AbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_RollerDrone();

private:

	UPROPERTY()
	class AREnemyBase* Enemy;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void RollUp(FGameplayEventData Payload);

	UFUNCTION()
	void RollUpFrameDelay();

	UFUNCTION()
	void UnRoll(FGameplayEventData Payload);

	UFUNCTION()
	void UnRollFrameDelay();

	UFUNCTION()
	void HideRoller(FGameplayEventData Payload);

	UFUNCTION()
	void ShowRoller(FGameplayEventData Payload);

	FDelegateHandle StunHandle;

	UPROPERTY()
	class USkeletalMeshComponent* RollerSkeletalMesh;

	UPROPERTY()
	bool bStunned;

	UFUNCTION()
	void StunChanged(bool bTased);
};
