// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/GA_ToniMelee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "Player/RPlayerBase.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ToniMelee::UGA_ToniMelee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetMeleeAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_ToniMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		K2_EndAbility();
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	bool bFound = false;
	float attackCooldown = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackCooldownReductionAttribute(), bFound);
	float attackSpeed = 1.0f;

	if (bFound == true)
	{
		attackSpeed = 3.0f - 2.0f * (attackCooldown - 0.1f) / (1.0f - 0.1f);
		//UE_LOG(LogTemp, Error, TEXT("Attack Speed: %f"), attackSpeed);
	}


	UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MeleeAttackAnim, attackSpeed);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ToniMelee::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ToniMelee::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ToniMelee::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ToniMelee::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ToniMelee::HandleDamage);
	WaitForDamage->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetExpTag());
	WaitForDamage2->EventReceived.AddDynamic(this, &UGA_ToniMelee::HandleDamage);
	WaitForDamage2->ReadyForActivation();

	TriggerAudioCue();
}

void UGA_ToniMelee::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		for (int32 i = 0; i < Payload.TargetData.Num(); ++i)
		{
			const FGameplayAbilityTargetData* Data = Payload.TargetData.Get(i);
			if (Data)
			{
				const TArray<TWeakObjectPtr<AActor>> Actors = Data->GetActors();
				for (const TWeakObjectPtr<AActor>& WeakActorPtr : Actors)
				{
					if (WeakActorPtr.IsValid())
					{
						ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(WeakActorPtr.Get());
						if (hitCharacter)
						{
							Player->HitMeleeAttack(hitCharacter);
						}
					}
				}
			}
		}
	}
}
