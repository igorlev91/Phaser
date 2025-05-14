// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/GA_BoltHead_Ultimate.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Player/Chester/ChesterBallActor.h"

#include "Targeting/RTargetActor_DotSpecial.h"
#include "Player/Chester/RChester_UltimateProj.h"

#include "Player/Chester/RTargetActor_ChesterUltimate.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"

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

UGA_BoltHead_Ultimate::UGA_BoltHead_Ultimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_BoltHead_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		K2_EndAbility();
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!CheckCooldown(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

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
					UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Ultimate::UltimateLerp, 1.0f, 0.0f, 1.0f));

					break;
				}
			}
		}
	}

	Player->ServerPlay_Torso_AnimMontage(Montage, 1.0f);

	FTimerHandle UltTimater;
	GetWorld()->GetTimerManager().SetTimer(UltTimater, this, &UGA_BoltHead_Ultimate::DoExplosion, 0.9f, false);

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}
}

void UGA_BoltHead_Ultimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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

void UGA_BoltHead_Ultimate::DoExplosion()
{
	if (K2_HasAuthority())
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(5000);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Player);

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_SpawnRadio(RadioSystem, Player, Player->GetActorLocation(), Player->GetActorLocation(), 500);
		}

		//DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 5000, 32, FColor::Red, false, 2.0f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Player->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<AREnemyBase*> alreadyDamaged;

		for (const FOverlapResult& result : OverlappingResults)
		{
			AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
			if (enemy)
			{
				if (alreadyDamaged.Contains(enemy) == false)
				{
					FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[0], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
					FGameplayEffectSpecHandle specHandle3 = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[2], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());

					FGameplayEffectSpec* spec = specHandle.Data.Get();
					FGameplayEffectSpec* spec3 = specHandle3.Data.Get();
					if (spec)
					{
						if (gameState)
						{
							gameState->Multicast_SpawnRadio(RadioSystem, enemy, enemy->GetActorLocation(), enemy->GetActorLocation(), 500);
						}

						alreadyDamaged.Add(enemy);
						enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					}

					if (spec3)
					{
						enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec3);
					}
				}
			}
		}


		FGameplayEffectSpecHandle specHandle2 = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(AttackDamages[1], 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());
		FGameplayEffectSpec* spec2 = specHandle2.Data.Get();
		if (spec2)
		{
			Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec2);
		}


		K2_EndAbility();
	}
}

void UGA_BoltHead_Ultimate::UltimateLerp(float value, float desiredValue, float times)
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

			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Ultimate::UltimateLerp, value, desiredValue, times));
		}
	}
}
