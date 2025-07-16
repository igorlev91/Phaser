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
#include "Player/BoltHead/RBoltHead_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

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
	//ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_BoltHead_Ultimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	/*if (!CheckCooldown(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}*/

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	TArray<USkeletalMeshComponent*> SkeletalComponents;
	Player->GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	for (USkeletalMeshComponent* Skeletal : SkeletalComponents)
	{
		if (Skeletal)
		{
			if (Skeletal->GetName() == TEXT("Torso"))
			{
				TorsoSkeletalMesh = Skeletal;
			}
		}
	}

	TArray<AActor*> BoltHeads;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARBoltHead_Actor::StaticClass(), BoltHeads);
	for (AActor* bolHead : BoltHeads)
	{
		ARBoltHead_Actor* boltHeadActor = Cast<ARBoltHead_Actor>(bolHead);
		if (boltHeadActor)
		{
			if (boltHeadActor->owningPlayer == Player)
			{
				BoltHead = boltHeadActor;
			}
		}
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
					UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Ultimate::UltimateLerp, 1.0f, 0.0f, 8.0f));

					break;
				}
			}
		}
	}

	spinAmount = 1.0f;
	FTimerHandle UltTimater;
	UltTimater = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Ultimate::DoUltimate, 8.0f));

	FTimerHandle LoopTimer;
	GetWorld()->GetTimerManager().SetTimer(LoopTimer, this, &UGA_BoltHead_Ultimate::HealingSpinCheck, 0.2f, true);

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

	if (K2_HasAuthority())
	{
		Player->ServerSpin_Torso(Player->GetMesh(), FRotator(0, -90, 0));
		Player->ServerSpin_Torso(TorsoSkeletalMesh, FRotator(0, -90, 0));

		if (BoltHead)
		{
			BoltHead->GetHeadMesh()->SetRelativeRotation(FRotator(0, 0, 0));
		}

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

void UGA_BoltHead_Ultimate::DoUltimate(float timeRemaining)
{
	if (K2_HasAuthority())
	{
		if (BoltHead && TorsoSkeletalMesh)
		{
			float DeltaYaw = spinAmount * GetWorld()->GetDeltaSeconds();

			// Spin the BoltHead mesh
			FRotator CurrentRotation = BoltHead->GetHeadMesh()->GetComponentRotation();
			CurrentRotation.Yaw += DeltaYaw;
			BoltHead->GetHeadMesh()->SetWorldRotation(CurrentRotation);

			// Spin the Torso mesh
			FRotator TorsoRotation = TorsoSkeletalMesh->GetComponentRotation();
			TorsoRotation.Yaw += DeltaYaw * 0.001f;

			Player->ServerSpin_Torso(TorsoSkeletalMesh, TorsoRotation);

			FRotator SpiderRotation = Player->GetMesh()->GetRelativeRotation();
			SpiderRotation.Yaw += DeltaYaw;

			Player->ServerSpin_Torso(Player->GetMesh(), SpiderRotation);

			spinAmount += 10.0f;
		}

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			FVector spawnPos = Player->GetActorLocation();
			spawnPos.Z -= 10.0f;

			gameState->Multicast_RequestSpawnVFXOnCharacter(BoltHeadSpinParticle, Player, spawnPos, Player->GetActorLocation(), 0);
		}

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Speedup, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

		SpinDamageCheck();

		timeRemaining -= GetWorld()->GetDeltaSeconds();

		if (timeRemaining > 0)
		{
			FTimerHandle UltTimater;
			UltTimater = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_BoltHead_Ultimate::DoUltimate, timeRemaining));
		}
		else
		{
			K2_EndAbility();
		}
	}
}

void UGA_BoltHead_Ultimate::HealingSpinCheck()
{
	if (K2_HasAuthority() && BoltHead)
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(1600);
		FCollisionQueryParams QueryParams;

		//DrawDebugSphere(GetWorld(), BoltHead->GetActorLocation(), 1600, 32, FColor::Red, false, 0.1f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, BoltHead->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<ARPlayerBase*> alreadyDamaged;

		for (const FOverlapResult& result : OverlappingResults)
		{
			ARPlayerBase* character = Cast<ARPlayerBase>(result.GetActor());
			if (character)
			{
				if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == true)
					continue;

				if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullHealthTag()) == true)
					continue;

				if (!alreadyDamaged.Contains(character))
				{
					alreadyDamaged.Add(character);

					FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
					FGameplayEffectSpecHandle effectSpechandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(UltimateHeal, 1.0f, contextHandle);

					FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
					if (spec)
					{
						AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
						if (gameState && BoltHead)
						{
							FVector spawnPos = BoltHead->GetActorLocation();
							spawnPos.Z += 35.0f;
							gameState->Multicast_ShootTexUltimate(UltimateHealParticle, BoltHead, spawnPos, character->GetActorLocation(), character->GetActorLocation());
						}

						bool bUltimateStrength = false;
						float ultimateStrength = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetUltimateStrengthAttribute(), bUltimateStrength);

						if (bUltimateStrength)
						{
							Player->AddToHealingDone(character, ultimateStrength / 0.75f);
						}

						character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					}
				}
			}
		}
	}
}

void UGA_BoltHead_Ultimate::SpinDamageCheck()
{
	if (K2_HasAuthority())
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(140);
		FCollisionQueryParams QueryParams;

		//DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 140, 32, FColor::Red, false, 0.1f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Player->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<ARCharacterBase*> alreadyDamaged;

		for (const FOverlapResult& result : OverlappingResults)
		{
			ARCharacterBase* character = Cast<ARCharacterBase>(result.GetActor());
			if (character)
			{
				if (character != Player)
				{
					if (!alreadyDamaged.Contains(character))
					{
						alreadyDamaged.Add(character);

						FVector LaunchVelocity = Player->GetActorForwardVector() * (1800.f);  // Adjust strength as needed
						LaunchVelocity.Z += (800.0f);
						character->LaunchBozo(LaunchVelocity);

						FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
						FGameplayEffectSpecHandle effectSpechandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(UltimateDamage, 1.0f, contextHandle);

						FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
						if (spec)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
							character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
							UAISense_Damage::ReportDamageEvent(this, character, Player, 1, Player->GetActorLocation(), Player->GetActorLocation());

							Player->DealtDamage(character); // make sure to do it afterwards so you can check health

							AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
							if (gameState)
							{
								gameState->Multicast_RobotGiblets(character->GetActorLocation(), -Player->GetActorForwardVector(), 6);
							}
						}
					}
				}
			}
		}
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
