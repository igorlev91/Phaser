// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_DeadlockSpecial.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Enemy/REnemyMissileProj.h"
#include "GameFramework/Character.h"

#include "BrainComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "AI/REnemyAIController.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/Chester/RChester_UltimateProj.h"

#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_DeadlockSpecial::UGA_DeadlockSpecial()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
}

void UGA_DeadlockSpecial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	EnemyAI = Cast<AAIController>(Enemy->GetController());
	if (EnemyAI == nullptr)
		return;

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_DeadlockSpecial::TryCommitAttack);
	WaitForActivation->ReadyForActivation();
}

void UGA_DeadlockSpecial::ApplyEffect(float value)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
	}

	FGameplayEffectSpecHandle fallSpec = MakeOutgoingGameplayEffectSpec(GravityClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	fallSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), value);

	GravityFallEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, fallSpec);
}

void UGA_DeadlockSpecial::TryCommitAttack(FGameplayEventData Payload)
{
	GetWorld()->GetTimerManager().SetTimer(StartWaitHandle, this, &UGA_DeadlockSpecial::DoLaunch, 1.0f, false);
	Enemy->ServerPlayAnimMontage(StartMontage);
}

void UGA_DeadlockSpecial::DoLaunch()
{
	if (Enemy)
	{
		Enemy->Multicast_SetDecal(DeadlockDecalMaterial);
	}

	if (AbilitySoundEfx.Num() > 0 && K2_HasAuthority())
	{
		if (40 >= FMath::RandRange(0, 99))
		{
			int32 Index = FMath::RandRange(0, AbilitySoundEfx.Num() - 1);
			USoundBase* SelectedSound = AbilitySoundEfx[Index];

			AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (GameState)
			{
				FVector Location = Enemy->GetActorLocation();
				FRotator Rotation = FRotator::ZeroRotator;
				USoundAttenuation* Attenuation = nullptr;

				GameState->Multicast_RequestPlayAudio(SelectedSound, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
			}
		}
	}

	if (K2_HasAuthority())
	{
		UObject* target = EnemyAI->GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
		TargetActor = Cast<AActor>(target);

		Enemy->SetCollisionStatus(false);

		FVector Direction = Enemy->GetActorUpVector();
		FVector LaunchVelocity = Direction * 3000.0f; //LaunchSpeed;

		Enemy->LaunchCharacter(LaunchVelocity, true, true);

		GetWorld()->GetTimerManager().SetTimer(EnablePhysicsLerpHandle, this, &UGA_DeadlockSpecial::EnablePhysicsDelay, 1.0f, false);
		LandingLerpHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DeadlockSpecial::TickLandOnPlayer, 4.0f));
	}
}

void UGA_DeadlockSpecial::TickLandOnPlayer(float timeRemaining)
{
	timeRemaining -= GetWorld()->GetDeltaSeconds();
	if (timeRemaining > 0)
	{
		if (TargetActor)
		{
			//UE_LOG(LogTemp, Error, TEXT("Landing: %f"), timeRemaining);

			FVector StartLocation = Enemy->GetActorLocation(); // Current location
			FVector TargetLocation = TargetActor->GetActorLocation(); // Target location

			FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, 0.5 * GetWorld()->GetDeltaSeconds());
			NewLocation.Z = StartLocation.Z; // Preserve the original Z

			Enemy->SetActorLocation(NewLocation);
		}

		LandingLerpHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DeadlockSpecial::TickLandOnPlayer, timeRemaining));
	}
	else
	{

		GetWorld()->GetTimerManager().SetTimer(LandingExplosionHandle, this, &UGA_DeadlockSpecial::LandingExplosion, 1.0f, false);

		FVector Direction = Enemy->GetActorUpVector() * -20000.0f;
		Enemy->LaunchCharacter(Direction, true, true);
		//ApplyEffect(50000.0f);
	}
}

void UGA_DeadlockSpecial::EnablePhysicsDelay()
{
	if (Enemy)
	{
		Enemy->SetCollisionStatus(true);
	}

}

void UGA_DeadlockSpecial::LandingExplosion()
{
	if (Enemy)
	{
		Enemy->Multicast_SetDecal(DefaultDecalMaterial);
	}

	Enemy->ServerPlayAnimMontage(EndMontage);

	FVector spawnPos = Enemy->GetActorLocation();
	spawnPos.Z -= 400.0f;

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState && ExplosionParticle)
	{
		gameState->Multicast_RequestSpawnVFX(ExplosionParticle, spawnPos, FVector::UpVector, 0);
		gameState->Multicast_RequestPlayAudio(ExplosionAudio, spawnPos, Enemy->GetActorRotation(), 1, 1, 0, ExplosionSoundAttenuationSettings);
	}

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(700);
	FCollisionQueryParams QueryParams;

	//DrawDebugSphere(GetWorld(), spawnPos, 1000, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, spawnPos, FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

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

				FGameplayEffectContextHandle contextHandle = Enemy->GetAbilitySystemComponent()->MakeEffectContext();
				FGameplayEffectSpecHandle explosionspecHandle = Enemy->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamage, 1.0f, contextHandle);

				FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
				if (newSpec)
				{
					alreadyDamaged.Add(player);

					FGameplayEffectSpecHandle specHandle = Enemy->GetAbilitySystemComponent()->MakeOutgoingSpec(FireEffect, 1.0f, Enemy->GetAbilitySystemComponent()->MakeEffectContext());
					FGameplayEffectSpec* spec = specHandle.Data.Get();
					if (spec)
					{
						player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					}

					//Enemy->ApplyItemEffectAtRandom(player, URAttributeSet::GetMaxHealthAttribute(), FireEffect);
					player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
				}
			}
		}
	}
}
