// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/GA_ToniPassive.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Framework/EOSPlayerState.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"

#include "Enemy/REnemyBase.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

UGA_ToniPassive::UGA_ToniPassive()
{
}

void UGA_ToniPassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	/*UAbilityTask_WaitGameplayEvent* EndTakeOffEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetApplyGravityJump());
	EndTakeOffEvent->EventReceived.AddDynamic(this, &UGA_ToniPassive::ApplyJump);
	EndTakeOffEvent->ReadyForActivation();*/

	Player->ServerPlayAnimMontage(DoubleJumpAnim);

	GetWorld()->GetTimerManager().SetTimer(ProcessDoubleJumpHandle, this, &UGA_ToniPassive::ApplyJump, 0.2f, false);
	ProcessDoubleJumpHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniPassive::DoubleJumpProcess));
}

void UGA_ToniPassive::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UGA_ToniPassive::ApplyJump(/*FGameplayEventData Payload*/)
{
	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	FVector LaunchVelocity = FVector(0.0f, 0.0f, 1700.0f); // Only Z movement
	Player->LaunchCharacter(LaunchVelocity, false, true);

	ShootBullets();

	FVector Start = Player->GetActorLocation() - FVector(0,0,800);
	TriggerAudioCue();

	float CapsuleRadius = 120.0f;
	float CapsuleHalfHeight = 800.0f;

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Capsule = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight / 2);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player); // Ignore self

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Start, FQuat(Player->GetActorRotation()), ECC_Pawn, Capsule, QueryParams);

	//DrawDebugCapsule(GetWorld(),Start,CapsuleHalfHeight,CapsuleRadius,FQuat::Identity,FColor::Cyan,false,2.0f,0,2.0f);

	for (const FOverlapResult& result : OverlappingResults)
	{
		UE_LOG(LogTemp, Error, TEXT("Hit enemy"));
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamage, 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());

			FGameplayEffectSpec* spec = specHandle.Data.Get();
			if (spec)
			{
				AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
				if (gameState)
				{
					gameState->Multicast_RobotGiblets(enemy->GetActorLocation(), enemy->GetActorUpVector(), enemy->GibletCount);
				}

				enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
				//Player->ApplyItemEffectAtRandom(enemy, URAttributeSet::GetMaxHealthAttribute(), FireUltimateEffect);
				Player->DealtDamage(enemy);
			}
		}
	}
}

void UGA_ToniPassive::ShootBullets()
{
	if (K2_HasAuthority())
	{
		FVector startL = Player->GetMesh()->GetSocketLocation("MeleeAim_L");
		FVector directionL = -Player->GetActorUpVector();
		FireAttack(startL, directionL);

		FVector startR = Player->GetMesh()->GetSocketLocation("MeleeAim_R");
		FVector directionR = -Player->GetActorUpVector();
		FireAttack(startR, directionR);

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();

		if (gameState)
		{
			FVector spawnPosR = Player->GetMesh()->GetSocketLocation("BulletCasing_R");
			FVector casingdirectionR = Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPosR, casingdirectionR, 102);

			FVector spawnPosL = Player->GetMesh()->GetSocketLocation("BulletCasing_L");
			FVector casingdirectionL = -Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPosL, casingdirectionL, 102);
		}
	}
}

void UGA_ToniPassive::FireAttack(FVector start, FVector direction)
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
	}
}

void UGA_ToniPassive::DoubleJumpProcess()
{
	if (!Player->GetCharacterMovement() || !Player->GetCharacterMovement()->IsFalling())
		K2_EndAbility();

	ProcessDoubleJumpHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniPassive::DoubleJumpProcess));
}
