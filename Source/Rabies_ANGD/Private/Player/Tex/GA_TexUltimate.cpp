// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexUltimate.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Framework/EOSActionGameState.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"


#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"
#include "Targeting/RTargetActor_DotSpecial.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

UGA_TexUltimate::UGA_TexUltimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	AbilityTags.AddTag(URAbilityGenericTags::GetUltimateAttackAimingTag());
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetUltimateAttackAimingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetUltimateAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_TexUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	if (Player)
	{
		TriggerAudioCue();
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}

	//UAbilityTask_WaitGameplayEvent* stopActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	//stopActivation->EventReceived.AddDynamic(this, &UGA_TexUltimate::FinishUltimate);
	//stopActivation->ReadyForActivation();
}

void UGA_TexUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		StopDurationAudioEffect();
		Player->ClientHitScan.Remove(ClientHitScanHandle);

	}
}

void UGA_TexUltimate::SendBounce()
{
	///////////////////////////////////////////////////////////// pierce through
	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = ECC_AllyRangedAttack;
	FCollisionQueryParams collisionParams;
	for (ARCharacterBase* hitEnemy : alreadyHitEnemies)
	{
		if (hitEnemy == nullptr)
			continue;
		collisionParams.AddIgnoredActor(hitEnemy);
	}
	FHitResult hitResult;

	FVector Direction = end - start;
	Direction.Normalize();
	FVector newEndPoint = end + (Direction * 9000.0f);

	bool hit = GetWorld()->SweepSingleByChannel(hitResult, end, newEndPoint, FQuat::FindBetweenVectors(FVector(1.0f, 1.0f, 1.0f), Direction), collisionChannel, collisionShape, collisionParams);
	if (hit)
	{
		FVector hitEnd = hitResult.ImpactPoint;
		RecieveAttackHitscan(hitResult.GetActor(), start, hitEnd, hasCrit);
	}
	//////////////////////////////////////////////////
}

void UGA_TexUltimate::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &UGA_TexUltimate::FinishTimerUltimate, 1.0f, false);

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Multicast_ShootTexUltimate(CritHitParticle, Player, startPos, endPos, endPos);
	}
	if (K2_HasAuthority())
	{
		start = startPos;
		end = endPos;
		hasCrit = bIsCrit;

		if (hitActor != nullptr)
		{
			if (hitActor != Player)
			{
				ARCharacterBase* hitEnemy = Cast<ARCharacterBase>(hitActor);
				if (hitEnemy == nullptr)
				{
					return;
				}

				if (alreadyHitEnemies.Contains(hitEnemy))
				{
					return;
				}

				alreadyHitEnemies.Add(hitEnemy);

				FGameplayEventData Payload = FGameplayEventData();

				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

				FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec((bIsCrit) ? CritUltimateDamage : UltimateDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

				Player->HitRangedAttack(hitEnemy); // make sure to do it afterwards so you can check health

				SignalDamageStimuliEvent(Payload.TargetData);

				/////////////////////////////////////////////////////////////

				FVector viewLoc;
				FRotator viewRot;

				Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

				// Find all actors in the world
				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(Player->playerController->GetWorld(), AREnemyBase::StaticClass(), AllActors);
				TArray<ARCharacterBase*> eligableTargets;

				for (AActor* Actor : AllActors)
				{
					ARCharacterBase* character = Cast<ARCharacterBase>(Actor);
					if (character == nullptr)
						continue;

					if (character == Player || alreadyHitEnemies.Contains(character))
						continue;

					if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
					{
						continue;
					}

					FVector DirectionToActor = (character->GetActorLocation() - viewLoc).GetSafeNormal();
					float DotProduct = FVector::DotProduct(viewRot.Vector(), DirectionToActor);
					float CosThreshold = FMath::Cos(FMath::DegreesToRadians(30.0f));

					if (DotProduct >= CosThreshold) // Actor is within the FOV angle
					{
						eligableTargets.Add(character);
					}
				}

				float closestDistance = 500000.0f;
				for (ARCharacterBase* target : eligableTargets)
				{
					if (target == nullptr)
						continue;

					float DistanceSquared = FVector::Dist(hitEnemy->GetActorLocation(), target->GetActorLocation());
					if (DistanceSquared < closestDistance)
					{
						closestDistance = DistanceSquared;
						start = hitEnemy->GetActorLocation();
						end = target->GetActorLocation();
					}
				}

				//GetWorld()->GetTimerManager().SetTimer(ShootingBounceTimer, this, &UGA_TexUltimate::SendBounce, 0.1f, false);
				ShootingBounceTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_TexUltimate::SendBounce));
				//SendBounce(hitEnemy, startPos, endPos, bIsCrit);
			}
		}
	}

}

void UGA_TexUltimate::FinishTimerUltimate()
{
	K2_EndAbility();
}

void UGA_TexUltimate::FinishUltimate(FGameplayEventData Payload)
{
	K2_EndAbility();
}
