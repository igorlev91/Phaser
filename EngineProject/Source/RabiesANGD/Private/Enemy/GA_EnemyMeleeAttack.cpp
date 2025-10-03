// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_EnemyMeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_EnemyMeleeAttack::UGA_EnemyMeleeAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetMeleeAttackCooldown());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetMeleeAttackCooldown());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_EnemyMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Deadlock SET UP ATTACKING"));
	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_EnemyMeleeAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_EnemyMeleeAttack::HandleDamage);
	WaitForDamage->ReadyForActivation();
}

void UGA_EnemyMeleeAttack::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Deadlock Attacking"));
		ARCharacterBase* character = Cast<ARCharacterBase>(GetOwningActorFromActorInfo());
		if (character)
		{
			TriggerAudioCue();

			character->ServerPlayAnimMontage(AttackAnim);
			if (AbilitySoundEfx.Num() > 0 && K2_HasAuthority())
			{
				if (40 <= FMath::RandRange(0, 99))
					return;

				AREnemyBase* myEnemy = Cast<AREnemyBase>(GetOwningActorFromActorInfo());
				if (myEnemy == nullptr)
					return;

				int32 Index = FMath::RandRange(0, AbilitySoundEfx.Num() - 1);
				USoundBase* SelectedSound = AbilitySoundEfx[Index];

				AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
				if (GameState)
				{
					FVector Location = myEnemy->GetActorLocation();
					FRotator Rotation = FRotator::ZeroRotator;
					USoundAttenuation* Attenuation = nullptr;

					GameState->Multicast_RequestPlayAudio(nullptr, SelectedSound, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
				}
			}

		}
	}
}

void UGA_EnemyMeleeAttack::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}
