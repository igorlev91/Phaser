// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_DeadlockRangedAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "AI/REnemyAIController.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Player/RPlayerBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_DeadlockRangedAttack::UGA_DeadlockRangedAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetRangedAttackCooldown());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetRangedAttackCooldown());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_DeadlockRangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Error, TEXT("Set up attack"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackRightActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_DeadlockRangedAttack::TryCommitRightAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForActivation2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackLeftActivationTag());
	WaitForActivation2->EventReceived.AddDynamic(this, &UGA_DeadlockRangedAttack::TryCommitLeftAttack);
	WaitForActivation2->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* AimingActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetStartAimingTag());
	AimingActivation->EventReceived.AddDynamic(this, &UGA_DeadlockRangedAttack::StartAiming);
	AimingActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* EndAimingActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAimingTag());
	EndAimingActivation->EventReceived.AddDynamic(this, &UGA_DeadlockRangedAttack::EndAiming);
	EndAimingActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_DeadlockRangedAttack::HandleDamage);
	WaitForDamage->ReadyForActivation();

	Character = Cast<AREnemyBase>(GetOwningActorFromActorInfo());

	if (Character)
	{
		ClientHitScanHandle = Character->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}

	TriggerAudioCue();
}

void UGA_DeadlockRangedAttack::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	if (K2_HasAuthority())
	{
		ExplosionHehe(endPos);

		if (hitActor == nullptr) return;
		if (hitActor != Character)
		{
			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState)
			{
				gameState->Multicast_ShootTexUltimate(ShootingParticle, Character, startPos, endPos, endPos);
			}

			ARPlayerBase* hitplayer = Cast<ARPlayerBase>(hitActor);
			if(hitplayer)
			{
				if (hitplayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetImmuneTag())
					|| hitplayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;
			}

			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_DeadlockRangedAttack::TryCommitLeftAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Character->RangedAttackSocketName = TEXT("Ranged_SocketLeft");
		//UE_LOG(LogTemp, Error, TEXT("DEADLOCKE Attacking"));
		TriggerAudioCue();
		Character->Hitscan(12000, nullptr);
	}
}

void UGA_DeadlockRangedAttack::TryCommitRightAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Character->RangedAttackSocketName = TEXT("Ranged_Socket");
		//UE_LOG(LogTemp, Error, TEXT("DEADLOCKE Attacking"));
		TriggerAudioCue();
		Character->Hitscan(120000, nullptr);
	}
}

void UGA_DeadlockRangedAttack::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_DeadlockRangedAttack::StartAiming(FGameplayEventData Payload)
{
	Character->UpdateAimingTagChange(true);

	if (AbilitySoundEfx.Num() > 0 && K2_HasAuthority())
	{
		if (40 <= FMath::RandRange(0, 99))
			return;

		int32 Index = FMath::RandRange(0, AbilitySoundEfx.Num() - 1);
		USoundBase* SelectedSound = AbilitySoundEfx[Index];

		AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (GameState)
		{
			FVector Location = Character->GetActorLocation();
			FRotator Rotation = FRotator::ZeroRotator;
			USoundAttenuation* Attenuation = nullptr;

			GameState->Multicast_RequestPlayAudio(SelectedSound, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
		}
	}



}

void UGA_DeadlockRangedAttack::EndAiming(FGameplayEventData Payload)
{
	Character->UpdateAimingTagChange(false);


}

void UGA_DeadlockRangedAttack::ExplosionHehe(FVector HitPos)
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState && ExplosionParticle)
	{
		gameState->Multicast_RequestSpawnVFX(ExplosionParticle, HitPos, FVector::UpVector, 0);
		gameState->Multicast_RequestPlayAudio(ExplosionAudio, HitPos, Character->GetActorRotation(), 1, 1, 0, ExplosionSoundAttenuationSettings);
	}

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(250);
	FCollisionQueryParams QueryParams;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 300, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, HitPos, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	TArray<ARPlayerBase*> alreadyDamaged;

	for (const FOverlapResult& result : OverlappingResults)
	{
		ARPlayerBase* player = Cast<ARPlayerBase>(result.GetActor());
		if (player)
		{
			if (!alreadyDamaged.Contains(player))
			{
				if (player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetImmuneTag())
					|| player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

				FGameplayEffectContextHandle contextHandle = Character->GetAbilitySystemComponent()->MakeEffectContext();
				FGameplayEffectSpecHandle explosionspecHandle = Character->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamage, 1.0f, contextHandle);

				FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
				if (newSpec)
				{
					alreadyDamaged.Add(player);
					player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
				}
			}
		}
	}
}
