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
#include "Enemy/REnemyBase.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

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

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackRightActivationTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ToniMelee::RightGun);
	WaitForDamage->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackLeftActivationTag());
	WaitForDamage2->EventReceived.AddDynamic(this, &UGA_ToniMelee::LeftGun);
	WaitForDamage2->ReadyForActivation();

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	TriggerAudioCue();
}

void UGA_ToniMelee::LeftGun(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FVector start = Player->GetMesh()->GetSocketLocation("MeleeAim_L");
		FVector direction = start - Player->GetMesh()->GetSocketLocation("Ranged_Socket");
		FireAttack(start, direction);

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			FVector spawnPos = Player->GetMesh()->GetSocketLocation("BulletCasing_L");
			FVector casingdirection = -Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPos, casingdirection, 102);
		}
	}
}

void UGA_ToniMelee::RightGun(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FVector start = Player->GetMesh()->GetSocketLocation("MeleeAim_R");
		FVector direction = start - Player->GetMesh()->GetSocketLocation("RangedAlt_Socket");
		FireAttack(start, direction);

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			FVector spawnPos = Player->GetMesh()->GetSocketLocation("BulletCasing_R");
			FVector casingdirection =  Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPos, casingdirection, 102);
		}
	}
}

void UGA_ToniMelee::FireAttack(FVector start, FVector direction)
{
	FVector endPos = start + direction * 8000.0f;

	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = ECC_AllyRangedAttack;

	FHitResult hitResult;
	bool hit = GetWorld()->SweepSingleByChannel(hitResult, start, endPos, FQuat::Identity, collisionChannel, collisionShape);
	
	if (hit)
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_ShootTexUltimate(ToniMeleeParticle, Player, start, hitResult.ImpactPoint, hitResult.ImpactPoint);
		}

		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(200);
		FCollisionQueryParams QueryParams;
		
		FVector hitEnd = hitResult.ImpactPoint;

		//DrawDebugSphere(GetWorld(), hitEnd, 200, 32, FColor::Red, false, 1.0f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, hitEnd, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<ARCharacterBase*> alreadyHit;

		for (const FOverlapResult& result : OverlappingResults)
		{
			AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
			ARPlayerBase* players = Cast<ARPlayerBase>(result.GetActor());
			if (enemy)
			{
				if (gameState)
				{
					gameState->Multicast_RobotGiblets(enemy->GetActorLocation(), enemy->GetActorUpVector(), enemy->GibletCount - 2);
				}

				if (!alreadyHit.Contains(enemy))
				{
					FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
					FGameplayEffectSpecHandle explosionspecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamage, 1.0f, contextHandle);

					FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
					if (newSpec)
					{
						alreadyHit.Add(enemy);
						enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
						Player->HitMeleeAttack(enemy);
					}
				}
			}
			if (players)
			{
				if (!alreadyHit.Contains(players))
				{
					FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
					FGameplayEffectSpecHandle explosionspecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(Speedup, 1.0f, contextHandle);

					FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
					if (newSpec)
					{
						alreadyHit.Add(players);
						players->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
					}
				}
			}
		}

	}
}