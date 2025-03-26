// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_Attack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Attack::UGA_Attack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitTargetAquiredEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitTargetAquiredEvent->EventReceived.AddDynamic(this, &UGA_Attack::HandleDamage);
	WaitTargetAquiredEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_Attack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAttackTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_Attack::StopAttacking);
	WaitStopEvent->ReadyForActivation();

	SetupWaitInputTask();
}

void UGA_Attack::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		ARCharacterBase* player = Cast<ARCharacterBase>(GetOwningActorFromActorInfo());
		if (player)
		{
			AActor* hitActor = player->Hitscan(300, 1);
			if (hitActor)
			{
				if (hitActor == GetOwningActorFromActorInfo() || !Cast<ARPlayerBase>(hitActor))
				{
					return;
				}

				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

				UE_LOG(LogTemp, Error, TEXT("Attacking FRIEND"));
				FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(DamageTest, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			}
		}
		//FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(DamageTest, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		//ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
		//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
	}
}

void UGA_Attack::TryCommitAttack(FGameplayEventData Payload)
{
	HandleDamage(Payload);
	bAttackCommitted = true;
}

void UGA_Attack::AbilityInputPressed(float TimeWaited)
{
	SetupWaitInputTask();
	TryCommitAttack(FGameplayEventData());
}

void UGA_Attack::SetupWaitInputTask()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Attack::AbilityInputPressed);
	WaitInputPress->ReadyForActivation();
}

void UGA_Attack::StopAttacking(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Error, TEXT("Attacking cancelled"));
	K2_EndAbility();
}
