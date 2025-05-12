// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MiniLock/GA_MiniLockSpecial.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Framework/EOSPlayerState.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Perception/AISense_Damage.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"
#include "Targeting/RTargetActor_DotSpecial.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UGA_MiniLockSpecial::UGA_MiniLockSpecial()
{
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetSpecialAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_MiniLockSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	if (K2_HasAuthority())
	{
		if (StartMontage)
		{
			Player->ServerPlayAnimMontage(StartMontage);
		}
		GetWorld()->GetTimerManager().SetTimer(JumpHandle, this, &UGA_MiniLockSpecial::DoJump, 1.0f, false);
	}
}

void UGA_MiniLockSpecial::DoJump()
{
	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	FVector Direction = Player->GetActorUpVector();
	FVector LaunchVelocity = Direction * 3000.0f; //LaunchSpeed;

	Player->LaunchCharacter(LaunchVelocity, true, true);
	GetWorld()->GetTimerManager().SetTimer(groundCheckHandle, this, &UGA_MiniLockSpecial::CheckGround, 0.2f, false);
}

void UGA_MiniLockSpecial::CheckGround()
{
	if (Player)
	{
		if (Player->GetCharacterMovement()->IsFalling())
		{
			groundCheckHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_MiniLockSpecial::CheckGround));
		}
		else
		{
			FVector spawnPos = Player->GetActorLocation();
			spawnPos.Z -= 100.0f;

			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState && ExplosionParticle)
			{
				gameState->Multicast_RequestSpawnVFX(ExplosionParticle, spawnPos, FVector::UpVector, 0);
				gameState->Multicast_RequestPlayAudio(ExplosionAudio, spawnPos, Player->GetActorRotation(), 1, 1, 0, ExplosionSoundAttenuationSettings);
			}

			if (EndMontage)
			{
				Player->ServerPlayAnimMontage(EndMontage);
			}

			TArray<FOverlapResult> OverlappingResults;

			FCollisionShape Sphere = FCollisionShape::MakeSphere(1000);
			FCollisionQueryParams QueryParams;

			//DrawDebugSphere(GetWorld(), spawnPos, 1000, 32, FColor::Red, false, 1.0f);

			bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, spawnPos, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

			TArray<AREnemyBase*> alreadyDamaged;

			for (const FOverlapResult& result : OverlappingResults)
			{
				AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
				if (enemy)
				{
					if (!alreadyDamaged.Contains(enemy))
					{
						FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
						FGameplayEffectSpecHandle explosionspecHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(SpecialDamage, 1.0f, contextHandle);

						FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
						if (newSpec)
						{
							alreadyDamaged.Add(enemy);

							FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(FireEffect, 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
							FGameplayEffectSpec* spec = specHandle.Data.Get();
							if (spec)
							{
								enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
							}

							//Enemy->ApplyItemEffectAtRandom(player, URAttributeSet::GetMaxHealthAttribute(), FireEffect);
							enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
							Player->HitSpecialAttack(enemy);
						}
					}
				}
			}
			K2_EndAbility();
		}
	}
}


void UGA_MiniLockSpecial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Error, TEXT("Ending ability"));
}