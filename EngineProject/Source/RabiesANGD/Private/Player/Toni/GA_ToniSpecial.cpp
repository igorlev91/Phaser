// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/GA_ToniSpecial.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Player/Toni/RTargetActor_ToniSpecial.h"

#include "Enemy/REnemyBase.h"

#include "Player/Tex/TexKnifesActor.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

UGA_ToniSpecial::UGA_ToniSpecial()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());
}

void UGA_ToniSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	directionRotLeft = 10.0f;
	directionRotRight = -10.0f;

	//bool bFound = false;
	//float attackCooldown = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetRangedAttackCooldownReductionAttribute(), bFound);

	/*if (bFound == true)
	{
		Player->attackSpeedBeforeSpecial = attackCooldown;
		UE_LOG(LogTemp, Error, TEXT("Attack Speed: %f"), attackSpeed);
	}*/

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackRightActivationTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_ToniSpecial::RightGun);
	WaitForDamage->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackLeftActivationTag());
	WaitForDamage2->EventReceived.AddDynamic(this, &UGA_ToniSpecial::LeftGun);
	WaitForDamage2->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PreparingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_ToniSpecial::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_ToniSpecial::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_ToniSpecial* dotPickActor = Cast<ARTargetActor_ToniSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetOwningPlayerControler(Player->playerController, -15);
		dotPickActor->SetTargettingRadus(500.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);

	for (int i = 1; i < 13; ++i)
	{
		float offset = 3.0f * (i - 5);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwningActorFromActorInfo();

		ARTargetActor_ToniSpecial* extraActor = GetWorld()->SpawnActor<ARTargetActor_ToniSpecial>(targetActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		ToniAbilityHints.Add(extraActor);


		if (extraActor)
		{
			extraActor->SetOwningPlayerControler(Player->playerController, offset);
			extraActor->SetTargettingRadus(500.0f);
		}
	}
}

void UGA_ToniSpecial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	for (ARTargetActor_ToniSpecial* toniAbility : ToniAbilityHints)
	{
		toniAbility->Destroy();
	}
	ToniAbilityHints.Empty();
}

void UGA_ToniSpecial::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (playTargettingMontageTask)
	{
		playTargettingMontageTask->OnBlendOut.RemoveAll(this);
		playTargettingMontageTask->OnInterrupted.RemoveAll(this);
		playTargettingMontageTask->OnCompleted.RemoveAll(this);
		playTargettingMontageTask->OnCancelled.RemoveAll(this);
	}

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	for (ARTargetActor_ToniSpecial* toniAbility : ToniAbilityHints)
	{
		toniAbility->Destroy();
	}
	ToniAbilityHints.Empty();

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			K2_EndAbility();
			return;
		}
	}

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_ToniSpecial::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	//K2_EndAbility();
}

void UGA_ToniSpecial::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//K2_EndAbility();
}

void UGA_ToniSpecial::LeftGun(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FVector start = Player->GetMesh()->GetSocketLocation("MeleeAim_L");

		FVector viewLoc;
		FRotator viewRot;
		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

		viewRot.Yaw += directionRotLeft;
		viewRot.Pitch += 2.5f;

		FireAttack(start, viewRot.Vector());
		directionRotLeft -= 6.0f;

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			FVector spawnPos = Player->GetMesh()->GetSocketLocation("BulletCasing_L");
			FVector casingdirection = -Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPos, casingdirection, 102);
		}
	}
}

void UGA_ToniSpecial::RightGun(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FVector start = Player->GetMesh()->GetSocketLocation("MeleeAim_R");

		FVector viewLoc;
		FRotator viewRot;
		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

		viewRot.Yaw += directionRotRight;
		viewRot.Pitch += 2.5f;

		FireAttack(start, viewRot.Vector());
		directionRotRight += 6.0f;

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			FVector spawnPos = Player->GetMesh()->GetSocketLocation("BulletCasing_R");
			FVector casingdirection = Player->GetActorRightVector();
			gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPos, casingdirection, 102);
		}
	}
}

void UGA_ToniSpecial::FireAttack(FVector start, FVector direction)
{
	TriggerAudioCue();

	FVector endPos = start + direction * 10000.0f;

	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = ECC_AllyRangedAttack;

	FHitResult hitResult;
	bool hit = GetWorld()->SweepSingleByChannel(hitResult, start, endPos, FQuat::Identity, collisionChannel, collisionShape);

	if (hit)
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_ShootTexUltimate(ToniSpecialParticle, Player, start, hitResult.ImpactPoint, hitResult.ImpactPoint);
		}

		if (hitResult.GetActor() == nullptr)
			return;

		AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitResult.GetActor());

		if (hitEnemy == nullptr)
			return;

		FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
		FGameplayEffectSpecHandle explosionspecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[0], 1.0f, contextHandle);

		FGameplayEffectSpecHandle AttackSpeedHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[1], 1.0f, contextHandle);

		FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
		if (newSpec)
		{
			hitEnemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
			Player->HitSpecialAttack(hitEnemy);
		}

		FGameplayEffectSpec* newAttackSpeedSpec = AttackSpeedHandle.Data.Get();
		if (newAttackSpeedSpec)
		{
			Player->StartToniRevertSpecialCooldown(); // this will
			Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newAttackSpeedSpec);
		}
	}
}