// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexUltimate.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


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

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

UGA_TexUltimate::UGA_TexUltimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	AbilityTags.AddTag(URAbilityGenericTags::GetUltimateAttackAimingTag());
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetUltimateAttackAimingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetUltimateAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_TexUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	if (Player)
	{
		TriggerAudioCue();
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}

	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		PostProcessVolume = *It;
	}

	if (PostProcessVolume)
	{
		FPostProcessSettings& Settings = PostProcessVolume->Settings;

		for (int32 i = 0; i < Settings.WeightedBlendables.Array.Num(); ++i)
		{
			UObject* BlendableObject = Settings.WeightedBlendables.Array[i].Object;

			if (BlendableObject && BlendableObject->IsA<UMaterialInterface>())
			{
				UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(BlendableObject);

				if (!DynamicMat)
				{
					UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(BlendableObject);
					DynamicMat = UMaterialInstanceDynamic::Create(MaterialInterface, this);
					Settings.WeightedBlendables.Array[i].Object = DynamicMat;
				}

				if (DynamicMat)
				{
					UltimateModeMat = DynamicMat;

					GetWorld()->GetTimerManager().ClearTimer(UltimateModeTimer);
					UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_TexUltimate::UltimateLerp, 1.0f, 0.0f, 1.0f));

					break;
				}
			}
		}
	}

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	//UAbilityTask_WaitGameplayEvent* stopActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetSpecialAttackActivationTag());
	//stopActivation->EventReceived.AddDynamic(this, &UGA_TexUltimate::FinishUltimate);
	//stopActivation->ReadyForActivation();
}

void UGA_TexUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		StopDurationAudioEffect();
		Player->ClientHitScan.Remove(ClientHitScanHandle);

	}

	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(UltimateModeTimer);
			PostProcessVolume->Settings.VignetteIntensity = 0.4f;
			PostProcessVolume->Settings.bOverride_SceneFringeIntensity = true;
			PostProcessVolume->Settings.bOverride_SceneFringeIntensity = 0.0f;
			PostProcessVolume->Settings.LensFlareIntensity = 0.0f;
			UltimateModeMat->SetScalarParameterValue(FName("Blend"), 1.0f);
		}
	}
}

void UGA_TexUltimate::SendBounce()
{
	///////////////////////////////////////////////////////////// pierce through
	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = ECC_AllyRangedAttack;
	FCollisionQueryParams collisionParams;
	for (ARCharacterBase* hitEnemy : alreadyHitEnemies)
	{
		if (hitEnemy == nullptr)
			continue;
		collisionParams.AddIgnoredActor(hitEnemy);
	}
	FHitResult hitResult;

	FVector Direction = end - start;
	Direction.Normalize();
	FVector newEndPoint = end + (Direction * 9000.0f);

	bool hit = GetWorld()->SweepSingleByChannel(hitResult, end, newEndPoint, FQuat::FindBetweenVectors(FVector(1.0f, 1.0f, 1.0f), Direction), collisionChannel, collisionShape, collisionParams);
	if (hit)
	{
		FVector hitEnd = hitResult.ImpactPoint;

		if (hasCrit)
		{
			FGameplayEffectSpecHandle EffectSpec2121 = MakeOutgoingGameplayEffectSpec(AddCritShot, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec2121);
		}

		RecieveAttackHitscan(hitResult.GetActor(), start, hitEnd, hasCrit);
	}
	//////////////////////////////////////////////////
}

void UGA_TexUltimate::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &UGA_TexUltimate::FinishTimerUltimate, 1.0f, false);

	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(UltimateModeTimer);
			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_TexUltimate::UltimateLerp, 0.0f, 1.0f, 1.0f));
		}
	}

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Multicast_ShootTexUltimate(CritHitParticle, Player, startPos, endPos, endPos);
	}
	if (K2_HasAuthority())
	{
		start = startPos;
		end = endPos;
		hasCrit = bIsCrit;

		if (hitActor != nullptr)
		{
			if (hitActor != Player)
			{
				ARCharacterBase* hitEnemy = Cast<ARCharacterBase>(hitActor);
				if (hitEnemy == nullptr)
				{
					return;
				}

				if (alreadyHitEnemies.Contains(hitEnemy))
				{
					return;
				}

				gameState->Multicast_RobotGiblets(endPos, hitActor->GetActorUpVector(), 6.0f);

				alreadyHitEnemies.Add(hitEnemy);

				FGameplayEventData Payload = FGameplayEventData();

				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

				FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec((bIsCrit) ? CritUltimateDamage : UltimateDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

				Player->HitRangedAttack(hitEnemy); // make sure to do it afterwards so you can check health

				SignalDamageStimuliEvent(Payload.TargetData);

				/////////////////////////////////////////////////////////////

				FVector viewLoc;
				FRotator viewRot;

				Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

				// Find all actors in the world
				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(Player->playerController->GetWorld(), AREnemyBase::StaticClass(), AllActors);
				TArray<ARCharacterBase*> eligableTargets;

				for (AActor* Actor : AllActors)
				{
					ARCharacterBase* character = Cast<ARCharacterBase>(Actor);
					if (character == nullptr)
						continue;

					if (character == Player || alreadyHitEnemies.Contains(character))
						continue;

					if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
					{
						continue;
					}

					FVector DirectionToActor = (character->GetActorLocation() - viewLoc).GetSafeNormal();
					float DotProduct = FVector::DotProduct(viewRot.Vector(), DirectionToActor);
					float CosThreshold = FMath::Cos(FMath::DegreesToRadians(30.0f));

					if (DotProduct >= CosThreshold) // Actor is within the FOV angle
					{
						eligableTargets.Add(character);
					}
				}

				float closestDistance = 500000.0f;
				for (ARCharacterBase* target : eligableTargets)
				{
					if (target == nullptr)
						continue;

					float DistanceSquared = FVector::Dist(hitEnemy->GetActorLocation(), target->GetActorLocation());
					if (DistanceSquared < closestDistance)
					{
						closestDistance = DistanceSquared;
						start = hitEnemy->GetActorLocation();
						end = target->GetActorLocation();
					}
				}

				//GetWorld()->GetTimerManager().SetTimer(ShootingBounceTimer, this, &UGA_TexUltimate::SendBounce, 0.1f, false);
				ShootingBounceTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_TexUltimate::SendBounce));
				//SendBounce(hitEnemy, startPos, endPos, bIsCrit);
			}
		}
	}

}

void UGA_TexUltimate::FinishTimerUltimate()
{
	K2_EndAbility();
}

void UGA_TexUltimate::FinishUltimate(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_TexUltimate::UltimateLerp(float value, float desiredValue, float times)
{
	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			float vignetteIntensity = FMathf::Lerp(PostProcessVolume->Settings.VignetteIntensity, (desiredValue == 1) ? 0.4f : 1, 3 * GetWorld()->GetDeltaSeconds());
			float sceneFringeIntensity = FMathf::Lerp(PostProcessVolume->Settings.SceneFringeIntensity, (desiredValue == 1) ? 0.0f : 2, 3 * GetWorld()->GetDeltaSeconds());
			float sceneLensFlare = FMathf::Lerp(PostProcessVolume->Settings.LensFlareIntensity, (desiredValue == 1) ? 0.0f : 0.01f, 3 * GetWorld()->GetDeltaSeconds());
			PostProcessVolume->Settings.VignetteIntensity = vignetteIntensity;
			PostProcessVolume->Settings.bOverride_SceneFringeIntensity = true;
			PostProcessVolume->Settings.SceneFringeIntensity = sceneFringeIntensity;
			PostProcessVolume->Settings.LensFlareIntensity = sceneLensFlare;

			value = FMath::Lerp(value, desiredValue, 3 * GetWorld()->GetDeltaSeconds());
			UltimateModeMat->SetScalarParameterValue(FName("Blend"), value);
			times -= GetWorld()->GetDeltaSeconds();

			if (times <= 0)
				return;

			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_TexUltimate::UltimateLerp, value, desiredValue, times));
		}
	}
}
