// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/REnemyMissileProj.h"
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

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

AREnemyMissileProj::AREnemyMissileProj()
{
}

void AREnemyMissileProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
{
	for (ARCharacterBase* character : AlreadyHitCharacters)
	{
		if (hitCharacter == character)
			return;
	}

	AlreadyHitCharacters.Add(hitCharacter);

	if (isEnemy == true)
	{
		return;
	}
	for (TSubclassOf<UGameplayEffect>& damageEffect : EffectsToApply)
	{

	}

	if (hitCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetImmuneTag())
		|| hitCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

	FGameplayEventData Payload = FGameplayEventData();

	FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle effectSpechandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[0], 1.0f, contextHandle);

	FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
	if (spec)
	{
		usingCharacter->HitSpecialAttack(hitCharacter);

		//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
		hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
	}
}

void AREnemyMissileProj::Explosion(ARCharacterBase* usingCharacter)
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

	FCollisionShape Sphere = FCollisionShape::MakeSphere(300);
	FCollisionQueryParams QueryParams;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 300, 32, FColor::Red, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

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

				FGameplayEffectContextHandle contextHandle = usingCharacter->GetAbilitySystemComponent()->MakeEffectContext();
				FGameplayEffectSpecHandle explosionspecHandle = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(EffectsToApply[0], 1.0f, contextHandle);

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

void AREnemyMissileProj::EnableHitDetection()
{
	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	if (MyMesh)
	{
		MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void AREnemyMissileProj::Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, ARCharacterBase* usingCharacter)
{
	GetWorldTimerManager().SetTimer(EnableTimerHandle, this, &AREnemyMissileProj::EnableHitDetection, 0.05f, false);

	UsingCharacter = usingCharacter;
	bNoMoreExplosion = false;
	EffectsToApply = effectsToApply;
	OnHitCharacter.AddUObject(this, &AREnemyMissileProj::HitCharacter);
}

void AREnemyMissileProj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentVelocity = ProjectileComponent->Velocity;
	if (CurrentVelocity.Length() <= 0.1f)
	{
		if (UsingCharacter && bNoMoreExplosion == false)
		{
			bNoMoreExplosion = true;
			Explosion(UsingCharacter);
		}
	}
}
