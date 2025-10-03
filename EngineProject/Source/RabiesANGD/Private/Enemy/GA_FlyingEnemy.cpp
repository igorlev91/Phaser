// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_FlyingEnemy.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Components/CapsuleComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "AI/REnemyAIController.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_FlyingEnemy::UGA_FlyingEnemy()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("stat.AI"));
	AbilityTags.AddTag(URAbilityGenericTags::GetFlyingTag());
	BlockAbilitiesWithTag.AddTag(URAbilityGenericTags::GetFlyingTag());
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetFlyingTag());
}

void UGA_FlyingEnemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	FlyingHeight = (bHighFlyer) ? FMath::RandRange(10000, 12000) : FMath::RandRange(3000, 8000);

	if (Enemy)
	{
		StunHandle = Enemy->OnTaserStatusChanged.AddLambda([this](bool bTased)
			{
				StunChanged(bTased);
			});
	}

	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetDeadTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_FlyingEnemy::EndFlying);
	WaitForActivation->ReadyForActivation();

	Enemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	Enemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	Enemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	Enemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);

	GetWorld()->GetTimerManager().SetTimer(EnableCollisionHandle, this, &UGA_FlyingEnemy::StartCollision, 1.0f, false);
	FlyingHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_FlyingEnemy::Fly));
}

void UGA_FlyingEnemy::StartCollision()
{
	Enemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	Enemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Enemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	Enemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}


void UGA_FlyingEnemy::EndFlying(FGameplayEventData Payload)
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Multicast_RequestSpawnVFXOnCharacter(DeathSmoke, Enemy, Enemy->GetActorLocation(), Enemy->GetActorLocation(), 0);
	}

	K2_EndAbility();
}

void UGA_FlyingEnemy::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (StunHandle.IsValid() && Enemy)
	{
		StopDurationAudioEffect();
		Enemy->OnTaserStatusChanged.Remove(StunHandle);
	}
}

void UGA_FlyingEnemy::StunChanged(bool bTased)
{
	UE_LOG(LogTemp, Error, TEXT("Changed stun changed tag"));
	bStunned = bTased;
}

void UGA_FlyingEnemy::Fly()
{
	if (Enemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		return;

	FVector Start = Enemy->GetActorLocation();
	FVector End = Start - FVector(0, 0, FlyingHeight); // Trace downward

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Enemy); // Ignore self

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility,QueryParams);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 0.1f);

	if (bHit)
	{
		float distanceToFloor = Start.Z - HitResult.ImpactPoint.Z;
		FVector currentVelocity = Enemy->GetVelocity();
		float currentGravity = Enemy->GetCharacterMovement()->GravityScale;

		float bobbingAcceleration = -3.0f * (distanceToFloor - bMaxHeight) - 1.5f * currentVelocity.Z;

		currentVelocity.Z += bobbingAcceleration * GetWorld()->GetDeltaSeconds(); // Euler Integration

		Enemy->GetCharacterMovement()->Velocity = currentVelocity;

		/*if (currentVelocity.Z <= 0)
		{
			float fallValue = currentVelocity.Z * 0.001f;

			FGameplayEffectSpecHandle fallSpec = MakeOutgoingGameplayEffectSpec(GravityFallClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			fallSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), fallValue);

			GravityFallEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, fallSpec);
		}*/
	}

	FlyingHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_FlyingEnemy::Fly));
}