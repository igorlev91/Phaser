// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/GA_BoltHead_Melee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "Player/RPlayerBase.h"
#include "Enemy/REnemyBase.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_BoltHead_Melee::UGA_BoltHead_Melee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetMeleeAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_BoltHead_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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


	Player->ServerPlay_Torso_AnimMontage(MeleeAttackAnim, attackSpeed);

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackRightActivationTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_BoltHead_Melee::FireSpark);
	WaitForDamage->ReadyForActivation();
	TriggerAudioCue();

	UAbilityTask_WaitGameplayEvent* WaitForDamage2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackLeftActivationTag());
	WaitForDamage2->EventReceived.AddDynamic(this, &UGA_BoltHead_Melee::FireVFX);
	WaitForDamage2->ReadyForActivation();
	TriggerAudioCue();

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 30);
		}
	}

	FTimerHandle endTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(endTimerHandle, this, &UGA_BoltHead_Melee::ForceEnd, 0.5f, false);

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	TriggerAudioCue();
}

void UGA_BoltHead_Melee::FireVFX(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_RequestSpawnVFXOnCharacter(MeleeStunParticle, Player, Player->GetActorLocation(), Player->GetActorLocation(), 0);
		}

		Player->StartCameraShake(5.3f, 0.3f);
	}
}

void UGA_BoltHead_Melee::FireSpark(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(1200);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Player);

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_SpawnRadio(RadioSystem, Player, Player->GetActorLocation(), Player->GetActorLocation(), 500);
		}

		//DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 1200, 32, FColor::Red, false, 2.0f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Player->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<AREnemyBase*> alreadyDamaged;

		for (const FOverlapResult& result : OverlappingResults)
		{
			AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
			if (enemy)
			{
				if (alreadyDamaged.Contains(enemy) == false)
				{
					FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[0], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
					FGameplayEffectSpecHandle specHandle3 = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[2], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());

					FGameplayEffectSpec* spec = specHandle.Data.Get();
					FGameplayEffectSpec* spec3 = specHandle3.Data.Get();
					if (spec)
					{
						if (gameState)
						{
							gameState->Multicast_SpawnRadio(RadioSystem, enemy, enemy->GetActorLocation(), enemy->GetActorLocation(), 500);
						}

						alreadyDamaged.Add(enemy);
						enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					}

					if (spec3)
					{
						enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec3);
					}
				}
			}
		}


		FGameplayEffectSpecHandle specHandle2 = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[1], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
		FGameplayEffectSpec* spec2 = specHandle2.Data.Get();
		if (spec2)
		{
			Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec2);
		}


		K2_EndAbility();
	}
}

void UGA_BoltHead_Melee::ForceEnd()
{
	K2_EndAbility();
}
