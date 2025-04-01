// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/RChester_UltimateProj.h"
#include "Character/RCharacterBase.h"
#include "Enemy/REnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemComponent.h"
#include "Framework/EOSActionGameState.h"
#include "Kismet/GameplayStatics.h"
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

ARChester_UltimateProj::ARChester_UltimateProj()
{
}

void ARChester_UltimateProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
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
	for (TSubclassOf<UGameplayEffect>& damageEffect : EffectsToApply)
	{

	}

	FGameplayEventData Payload = FGameplayEventData();

	FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle effectSpechandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[0], 1.0f, contextHandle);

	FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
	if (spec)
	{
		usingCharacter->HitSpecialAttack(hitCharacter);

		//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
		hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);

		UAISense_Damage::ReportDamageEvent(this, hitCharacter, usingCharacter, 1, hitCharacter->GetActorLocation(), hitCharacter->GetActorLocation());
	}
}

void ARChester_UltimateProj::Explosion(ARCharacterBase* usingCharacter)
{
	GetWorld()->GetTimerManager().SetTimer(NewDestroyHandle, this, &ARProjectileBase::DestroySelf, 0.3f, false);

	//AudioComp->SetSound(ExplosionAudio);
	//AudioComp->Play();

	//void PlaySoundAtLocation(const UObject * WorldContextObject, USoundBase * Sound, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency * ConcurrencySettings = nullptr, const AActor * OwningActor = nullptr, const UInitialActiveSoundParams * InitialParams = nullptr);


	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState && HitExplosion)
	{
		FVector spawnPos = GetActorLocation();
		gameState->Multicast_RequestSpawnVFX(HitExplosion, spawnPos, FVector::UpVector, 0);
		gameState->Multicast_RequestPlayAudio(ExplosionAudio, GetActorLocation(), GetActorRotation(), 1, 1, 0, ExplosionSoundAttenuationSettings);
	}

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(500);
	FCollisionQueryParams QueryParams;

	DrawDebugSphere(GetWorld(), GetActorLocation(), 500, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	TArray<AREnemyBase*> alreadyDamaged;

	for (const FOverlapResult& result : OverlappingResults)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			if (!alreadyDamaged.Contains(enemy))
			{
				FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
				FGameplayEffectSpecHandle explosionspecHandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[1], 1.0f, contextHandle);

				FGameplayEffectSpec* newSpec = explosionspecHandle.Data.Get();
				if (newSpec)
				{
					alreadyDamaged.Add(enemy);
					enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*newSpec);
				}
			}
		}
	}
}

void ARChester_UltimateProj::EnableHitDetection()
{
	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	if (MyMesh)
	{
		MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ARChester_UltimateProj::Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter)
{
	GetWorldTimerManager().SetTimer(EnableTimerHandle, this, &ARChester_UltimateProj::EnableHitDetection, 0.05f, false);

	UsingCharacter = usingCharacter;
	bNoMoreExplosion = false;
	EffectsToApply = effectsToApply;
	OnHitCharacter.AddUObject(this, &ARChester_UltimateProj::HitCharacter);
}

void ARChester_UltimateProj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = ProjectileComponent->Velocity;
	if (CurrentVelocity.Length() <= 0)
	{
		if (UsingCharacter && bNoMoreExplosion == false)
		{
			bNoMoreExplosion = true;
			Explosion(UsingCharacter);
		}
	}
}
