// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_ArmadillioHeadbutt.h"

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

UGA_ArmadillioHeadbutt::UGA_ArmadillioHeadbutt()
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

void UGA_ArmadillioHeadbutt::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Enemy = Cast<AREnemyBase>(GetOwningActorFromActorInfo());
	if (Enemy == nullptr)
		return;


	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_ArmadillioHeadbutt::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForLungeActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetShowRollerTag());
	WaitForLungeActivation->EventReceived.AddDynamic(this, &UGA_ArmadillioHeadbutt::Lunge);
	WaitForLungeActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ArmadillioHeadbutt::HandleDamage);
	WaitForDamage->ReadyForActivation();
}

void UGA_ArmadillioHeadbutt::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
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

void UGA_ArmadillioHeadbutt::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_ArmadillioHeadbutt::Lunge(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (GameState)
		{
			FVector Location = Enemy->GetActorLocation();
			FRotator Rotation = FRotator::ZeroRotator;
			USoundAttenuation* Attenuation = nullptr;

			GameState->Multicast_RequestPlayAudio(nullptr, AbilitySoundEfx[0], Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
		}

		FVector LaunchVelocity = Enemy->GetActorForwardVector() * (700.f);  // Adjust strength as needed
		LaunchVelocity.Z += (200.0f);
		Enemy->LaunchBozo(LaunchVelocity);
	}
}
