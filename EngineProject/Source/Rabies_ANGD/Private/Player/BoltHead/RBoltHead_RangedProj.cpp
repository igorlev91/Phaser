// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/RBoltHead_RangedProj.h"
#include "Character/RCharacterBase.h"
#include "Enemy/REnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemComponent.h"
#include "Framework/EOSActionGameState.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

void ARBoltHead_RangedProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
{
	for (ARCharacterBase* character : AlreadyHitCharacters)
	{
		if (hitCharacter == character)
			return;
	}

	AlreadyHitCharacters.Add(hitCharacter);

	if (isEnemy == false)
	{
		return;
	}

	FGameplayEventData Payload = FGameplayEventData();

	FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle effectSpechandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[0], 1.0f, contextHandle);

	FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
	if (spec)
	{

		//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
		hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		usingCharacter->HitRangedAttack(hitCharacter);

		UAISense_Damage::ReportDamageEvent(this, hitCharacter, usingCharacter, 1, hitCharacter->GetActorLocation(), hitCharacter->GetActorLocation());
	}
}

void ARBoltHead_RangedProj::Explosion()
{
	if (HasAuthority() == false)
		return;

	FTimerHandle NewDestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(NewDestroyHandle, this, &ARProjectileBase::DestroySelf, 0.3f, false);

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState && HitExplosion)
	{
		FVector spawnPos = GetActorLocation();
		gameState->Multicast_RequestSpawnVFX(HitExplosion, spawnPos, FVector::UpVector, 0);
		gameState->Multicast_RequestPlayAudio(ExplosionAudio, GetActorLocation(), GetActorRotation(), 1, 1, 0, ExplosionSoundAttenuationSettings);
	}

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(300);
	FCollisionQueryParams QueryParams;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 300, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	TArray<AREnemyBase*> alreadyDamaged;

	for (const FOverlapResult& result : OverlappingResults)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			if (gameState)
			{
				gameState->Multicast_RobotGiblets(enemy->GetActorLocation(), enemy->GetActorUpVector(), enemy->GibletCount * 1.0f);
			}

			if (!alreadyDamaged.Contains(enemy))
			{
				FGameplayEffectContextHandle contextHandle = UsingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
				FGameplayEffectSpecHandle explosionspecHandle = UsingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[0], 1.0f, contextHandle);

				FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
				if (newSpec)
				{
					alreadyDamaged.Add(enemy);
					enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);

					UsingCharacter->HitRangedAttack(enemy);
				}
			}
		}
	}
}

void ARBoltHead_RangedProj::BounceCharacter(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{

}

void ARBoltHead_RangedProj::EnableHitDetection()
{
	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	if (MyMesh)
	{
		MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ARBoltHead_RangedProj::Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter)
{

	EffectsToApply = effectsToApply;

	UsingCharacter = usingCharacter;

	ProjMesh->IgnoreActorWhenMoving(usingCharacter, true);

	GetWorldTimerManager().SetTimer(EnableTimerHandle, this, &ARBoltHead_RangedProj::EnableHitDetection, 0.03f, false);

	OnBounceCharacter.AddUObject(this, &ARBoltHead_RangedProj::BounceCharacter);
	OnHitCharacter.AddUObject(this, &ARBoltHead_RangedProj::HitCharacter);
}

void ARBoltHead_RangedProj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = ProjectileComponent->Velocity;
	if (CurrentVelocity.Length() <= 0.1f)
	{
		if (bNoMoreExplosion == false)
		{
			bNoMoreExplosion = true;
			Explosion();
		}
	}

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(900);
	FCollisionQueryParams QueryParams;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 900, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	AREnemyBase* foundEnemy = nullptr;
	float nearestEnemy = 500000.0f;

	for (const FOverlapResult& result : OverlappingResults)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			if (enemy == nullptr)
				continue;

			if (enemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
				continue;

			float DistanceSquared = FVector::Dist(GetActorLocation(), enemy->GetActorLocation());
			if (DistanceSquared < nearestEnemy)
			{
				nearestEnemy = DistanceSquared;
				foundEnemy = enemy;
			}
		}
	}

	if (foundEnemy != nullptr)
	{
		FRotator CurrentRotation = GetActorRotation(); // Or your component's rotation

		FVector Direction = (foundEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();

		float RotationSpeed = 20.0f;

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

		SetActorRotation(NewRotation);


		float TargetSpeed = ProjectileComponent->Velocity.Size(); // Or a fixed speed like 2000.f
		FVector DirectionVelocity = GetActorForwardVector().GetSafeNormal();
		ProjectileComponent->Velocity = DirectionVelocity * TargetSpeed;


	}
}
