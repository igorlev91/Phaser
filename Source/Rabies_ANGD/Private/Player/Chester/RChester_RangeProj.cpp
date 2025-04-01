// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/RChester_RangeProj.h"
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

#include "Framework/EOSActionGameState.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

void ARChester_RangeProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
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
		usingCharacter->HitRangedAttack(hitCharacter);

		//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
		hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);

		UAISense_Damage::ReportDamageEvent(this, hitCharacter, usingCharacter, 1, hitCharacter->GetActorLocation(), hitCharacter->GetActorLocation());
	}
}

void ARChester_RangeProj::BounceCharacter(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	/*AREnemyBase* enemy = Cast<AREnemyBase>(ImpactResult.GetActor());
	if (enemy)
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(1200);
		FCollisionQueryParams QueryParams;

		DrawDebugSphere(GetWorld(), GetActorLocation(), 1200, 32, FColor::Blue, false, 0.2f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		for (const FOverlapResult& result : OverlappingResults)
		{
			AREnemyBase* enemy2 = Cast<AREnemyBase>(result.GetActor());
			if (enemy2)
			{
				FVector dirToTarget = enemy2->GetActorLocation() - enemy->GetActorLocation();
				dirToTarget.Normalize();
				FVector NewVelocity = dirToTarget * ImpactVelocity;
				ProjectileComponent->Velocity = NewVelocity;
			}
		}
	}*/
}

void ARChester_RangeProj::Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter)
{
	EffectsToApply = effectsToApply;

	OnBounceCharacter.AddUObject(this, &ARChester_RangeProj::BounceCharacter);
	OnHitCharacter.AddUObject(this, &ARChester_RangeProj::HitCharacter);
}
