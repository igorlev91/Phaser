// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/RTex_SpecialProj.h"
#include "Character/RCharacterBase.h"
#include "Enemy/REnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemComponent.h"

#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Framework/EOSActionGameState.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void ARTex_SpecialProj::HitCharacter(ARCharacterBase* usingCharacter, ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
{
	for (ARCharacterBase* character : AlreadyHitCharacters)
	{
		if (hitCharacter == character)
			return;
	}
	DisableHitDetection();

	this->AttachToActor(hitCharacter, FAttachmentTransformRules::KeepWorldTransform);

	AlreadyHitCharacters.Add(hitCharacter);

	if (isEnemy == true)
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_RobotGiblets(hitCharacter->GetActorLocation(), hitCharacter->GetActorUpVector(), 2);
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

			if (!UltimateCooldownClass) 
				return;

			// Query for the active cooldown effect
			FGameplayEffectQuery Query;
			Query.EffectDefinition = UltimateCooldownClass;

			TArray<FActiveGameplayEffectHandle> ActiveEffects = usingCharacter->GetAbilitySystemComponent()->GetActiveEffects(Query);
			for (FActiveGameplayEffectHandle Handle : ActiveEffects)
			{
				const FActiveGameplayEffect* activeEffect = usingCharacter->GetAbilitySystemComponent()->GetActiveGameplayEffect(Handle);
				if (activeEffect)
				{
					float remainingTime = activeEffect->GetTimeRemaining(usingCharacter->GetAbilitySystemComponent()->GetWorld()->GetTimeSeconds());
					if (remainingTime <= 0)
						return;

					float NewTime = remainingTime - 2.5f;
					NewTime = FMathf::Clamp(NewTime, 0.01f, NewTime);

					usingCharacter->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(Handle);

					// Reapply a new cooldown effect with adjusted duration
					if (NewTime >= 0)
					{
						FGameplayEffectSpecHandle NewCooldownSpec = usingCharacter->GetAbilitySystemComponent()->MakeOutgoingSpec(UltimateCooldownClass, 1, usingCharacter->GetAbilitySystemComponent()->MakeEffectContext());
						NewCooldownSpec.Data->SetDuration(NewTime, true);
						usingCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*NewCooldownSpec.Data);
					}
				}
			}
		}
	}
}

void ARTex_SpecialProj::EnableHitDetection()
{
	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	if (MyMesh)
	{
		MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ARTex_SpecialProj::Init(TArray<TSubclassOf<class UGameplayEffect>> effectsToApply, TSubclassOf<class UGameplayEffect> cooldownReduction)
{
	EffectsToApply = effectsToApply;
	OnHitCharacter.AddUObject(this, &ARTex_SpecialProj::HitCharacter);
	UltimateCooldownClass = cooldownReduction;

	UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	if (MyMesh)
	{
		MyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	GetWorldTimerManager().SetTimer(EnableTimerHandle, this, &ARTex_SpecialProj::EnableHitDetection, 0.07f, false);
	//OnActorHit.AddDynamic(this, &ARTex_SpecialProj::OnHit);
}
