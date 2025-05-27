// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MiniLock/GA_MiniLockUltimate.h"

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

UGA_MiniLockUltimate::UGA_MiniLockUltimate()
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

void UGA_MiniLockUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	if (Player)
	{
		GetWorld()->GetTimerManager().SetTimer(StopUltimateTimer, this, &UGA_MiniLockUltimate::FinishTimerUltimate, 8.0f, false);
		GetWorld()->GetTimerManager().SetTimer(StopUltimateMode, this, &UGA_MiniLockUltimate::FinishUltimateMode, 7.5f, false);

		TriggerAudioCue();
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
					UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_MiniLockUltimate::UltimateLerp, 1.0f, 0.0f, 1.0f));

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

	Player->ServerPlayAnimMontage(CastingMontage);

	FTimerHandle ultHandle;
	FTimerHandle ultHandle2;
	GetWorld()->GetTimerManager().SetTimer(ultHandle2, this, &UGA_MiniLockUltimate::CastShockwaveUltimate, 0.2f, false);
	GetWorld()->GetTimerManager().SetTimer(ultHandle, this, &UGA_MiniLockUltimate::CastUltimate, 0.4f, false);
}

void UGA_MiniLockUltimate::FinishTimerUltimate()
{
	K2_EndAbility();
}

void UGA_MiniLockUltimate::FinishUltimateMode()
{
	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(UltimateModeTimer);
			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_MiniLockUltimate::UltimateLerp, 0.0f, 1.0f, 1.0f));
		}
	}
}

void UGA_MiniLockUltimate::FinishUltimate(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_MiniLockUltimate::CastUltimate()
{
	if (K2_HasAuthority())
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();

		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(1000);
		FCollisionQueryParams QueryParams;

		//DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 1000, 32, FColor::Green, false, 0.1f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Player->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<ARPlayerBase*> alreadyHealedPlayers;

		for (const FOverlapResult& result : OverlappingResults)
		{
			ARPlayerBase* player = Cast<ARPlayerBase>(result.GetActor());
			if (player)
			{
				if (alreadyHealedPlayers.Contains(player) == false)
				{

					FGameplayEffectSpecHandle specHandle = player->GetAbilitySystemComponent()->MakeOutgoingSpec(Buff, 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());

					FGameplayEffectSpec* spec = specHandle.Data.Get();
					if (spec)
					{
						if (gameState && LingerParticle)
						{
							gameState->Multicast_RequestSpawnVFXOnCharacter(LingerParticle, player, player->GetActorLocation(), player->GetActorLocation(), 0);
						}

						alreadyHealedPlayers.Add(player);
						player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					}
				}
			}
		}
	}
}

void UGA_MiniLockUltimate::CastShockwaveUltimate()
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();

	if (gameState && CastParticle)
	{
		gameState->Multicast_RequestSpawnVFXOnCharacter(CastParticle, Player, Player->GetActorLocation(), Player->GetActorLocation(), 0);
	}
}

void UGA_MiniLockUltimate::UltimateLerp(float value, float desiredValue, float times)
{
	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			float vignetteIntensity = FMath::Lerp(PostProcessVolume->Settings.VignetteIntensity, (desiredValue == 1) ? 0.4f : 1, 3 * GetWorld()->GetDeltaSeconds());
			float sceneFringeIntensity = FMath::Lerp(PostProcessVolume->Settings.SceneFringeIntensity, (desiredValue == 1) ? 0.0f : 2, 3 * GetWorld()->GetDeltaSeconds());
			float sceneLensFlare = FMath::Lerp(PostProcessVolume->Settings.LensFlareIntensity, (desiredValue == 1) ? 0.0f : 0.01f, 3 * GetWorld()->GetDeltaSeconds());
			PostProcessVolume->Settings.VignetteIntensity = vignetteIntensity;
			PostProcessVolume->Settings.bOverride_SceneFringeIntensity = true;
			PostProcessVolume->Settings.SceneFringeIntensity = sceneFringeIntensity;
			PostProcessVolume->Settings.LensFlareIntensity = sceneLensFlare;

			value = FMath::Lerp(value, desiredValue, 3 * GetWorld()->GetDeltaSeconds());
			UltimateModeMat->SetScalarParameterValue(FName("Blend"), value);
			times -= GetWorld()->GetDeltaSeconds();

			if (times <= 0)
				return;

			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_MiniLockUltimate::UltimateLerp, value, desiredValue, times));
		}
	}
}

void UGA_MiniLockUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

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