// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/GA_ToniUltimate.h"

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
#include "GA_ToniUltimate.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#define ECC_AllyRangedAttack ECC_GameTraceChannel3

UGA_ToniUltimate::UGA_ToniUltimate()
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

void UGA_ToniUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());

	if (Player)
	{
		GetWorld()->GetTimerManager().SetTimer(StopUltimateTimer, this, &UGA_ToniUltimate::FinishTimerUltimate, 8.0f, false);
		GetWorld()->GetTimerManager().SetTimer(StopUltimateMode, this, &UGA_ToniUltimate::FinishUltimateMode, 7.5f, false);

		//TriggerAudioCue();
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
					UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::UltimateLerp, 1.0f, 0.0f, 1.0f));

					break;
				}
			}
		}
	}

	Player->ServerPlayAnimMontage(StartupMontage);

	HoldTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::TryAttack, 0.3f));

	GetWorld()->GetTimerManager().SetTimer(spinhandle, this, &UGA_ToniUltimate::TrySwpin, 0.2f, true);

	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Speedup, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		SpeedupEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

		if (AbilityStartSoundEfx.Num() > 0)
		{
			if (2 <= FMath::RandRange(0, 99))
			{
				Player->PlayVoiceLine(AbilityStartSoundEfx, 100);
			}
			else
			{
				Player->PlayVoiceLine(AbilityMemeStartSoundEfx, 100);
			}
		}
	}

	UAbilityTask_WaitGameplayEvent* stopActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateHold());
	stopActivation->EventReceived.AddDynamic(this, &UGA_ToniUltimate::RecieveAttack);
	stopActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* stopActivation2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateRelease());
	stopActivation2->EventReceived.AddDynamic(this, &UGA_ToniUltimate::ReleaseAttack);
	stopActivation2->ReadyForActivation();
}

void UGA_ToniUltimate::RecieveAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (Player->GetPlayerBaseState() == nullptr)
			return;

		if (Player->GetPlayerBaseState()->GetHoldingUltimate())
		{
			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Slowdown, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			SlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
			//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);

			Player->ServerStopAnimMontage(EndingMontage);
			Player->ServerPlayAnimMontage(CastingMontage);

			if (K2_HasAuthority())
			{
				if (AbilityActivateSoundEfx.Num() > 0)
				{
					Player->PlayVoiceLine(AbilityActivateSoundEfx, 100);
				}
			}

		}
	}
}

void UGA_ToniUltimate::ReleaseAttack(FGameplayEventData Payload)
{
	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (AnimInstance->Montage_IsPlaying(CastingMontage))
		{
			Player->ServerStopAnimMontage(CastingMontage);
			Player->ServerPlayAnimMontage(EndingMontage);

			if (K2_HasAuthority())
			{
				FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Slowdown, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
				SlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
				//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			}
		}
	}
}

void UGA_ToniUltimate::TryAttack(float timer)
{
	if (K2_HasAuthority() && Player->GetPlayerBaseState() != nullptr)
	{

		UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (Player->GetPlayerBaseState()->GetHoldingUltimate() == false || AnimInstance->Montage_IsPlaying(CastingMontage) == false)
			{
				HoldTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::TryAttack, timer));
				return;
			}
		}

		if (timer > 0)
		{
			timer -= GetWorld()->GetDeltaSeconds();
			HoldTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::TryAttack, timer));
			return;
		}

		HoldTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::TryAttack, 0.3f));

		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(1400);
		FCollisionQueryParams QueryParams;

		//DrawDebugSphere(GetWorld(), Player->GetActorLocation(), 1400, 32, FColor::Red, false, 1.0f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, Player->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<AREnemyBase*> alreadyDamaged;

		for (const FOverlapResult& result : OverlappingResults)
		{
			AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
			if (enemy)
			{
				if (!alreadyDamaged.Contains(enemy))
				{
					alreadyDamaged.Add(enemy);
					FVector startPos = (Player->DoAltAiming) ? Player->GetPlayerBaseState()->GetRangedLocation() : Player->GetPlayerBaseState()->GetAltRangedLocation();

					GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::LaunchAttack, enemy, FMath::RandRange(1, 20)));
				}
			}
		}
	}
}

void UGA_ToniUltimate::TrySwpin()
{
	if (K2_HasAuthority() == false)
		return;

	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
	if (Player->GetPlayerBaseState()->GetHoldingUltimate() == false || AnimInstance->Montage_IsPlaying(CastingMontage) == false)
		return;

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		FVector spawnPos = Player->GetActorLocation();
		spawnPos.Z -= 80.0f;

		gameState->Multicast_RequestSpawnVFXOnCharacter(ToniSpinParticle, Player, spawnPos, Player->GetActorLocation(), 0);
	}
}

void UGA_ToniUltimate::LaunchAttack(AREnemyBase* hittingEnemy, int timer)
{

	if (timer > 0)
	{
		timer--;
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::LaunchAttack, hittingEnemy, timer));
		return;
	}

	TriggerAudioCue();

	FVector startPos = (Player->DoAltAiming) ? Player->GetPlayerBaseState()->GetRangedLocation() : Player->GetPlayerBaseState()->GetAltRangedLocation();

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Multicast_ShootTexUltimate(ToniHitParticle, Player, startPos, hittingEnemy->GetActorLocation(), hittingEnemy->GetActorLocation());
		gameState->Multicast_RobotGiblets(hittingEnemy->GetActorLocation(), -Player->GetActorForwardVector(), hittingEnemy->GibletCount - 1.0f);
		FVector spawnPos = (Player->DoAltAiming) ? Player->GetMesh()->GetSocketLocation("BulletCasing_L") : Player->GetMesh()->GetSocketLocation("BulletCasing_R");
		FVector direction = (Player->DoAltAiming) ? -Player->GetActorRightVector() : Player->GetActorRightVector();
		gameState->Multicast_RequestSpawnVFX(BulletCasing, spawnPos, direction, 102);
	}

	FGameplayEventData Payload = FGameplayEventData();

	FGameplayEffectContextHandle contextHandle = Player->GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle effectSpechandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(UltimateDamage, 1.0f, contextHandle);

	FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
	if (spec)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Damaging Target"));
		hittingEnemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		UAISense_Damage::ReportDamageEvent(this, hittingEnemy, Player, 1, Player->GetActorLocation(), Player->GetActorLocation());

		Player->DealtDamage(hittingEnemy); // make sure to do it afterwards so you can check health
	}
}

void UGA_ToniUltimate::FinishTimerUltimate()
{
	K2_EndAbility();
}

void UGA_ToniUltimate::FinishUltimateMode()
{

	if (PostProcessVolume)
	{
		if (UltimateModeMat != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(UltimateModeTimer);
			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::UltimateLerp, 0.0f, 1.0f, 1.0f));
		}
	}
}

void UGA_ToniUltimate::FinishUltimate(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_ToniUltimate::UltimateLerp(float value, float desiredValue, float times)
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

			UltimateModeTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_ToniUltimate::UltimateLerp, value, desiredValue, times));
		}
	}
}


void UGA_ToniUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if(AnimInstance->Montage_IsPlaying(CastingMontage))
		{ 
			Player->ServerStopAnimMontage(CastingMontage);
			Player->ServerPlayAnimMontage(EndingMontage);

			if (K2_HasAuthority())
			{
				UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
				if (ASC)
				{
					ASC->RemoveActiveGameplayEffect(SpeedupEffectHandle);
				}

				FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(Slowdown, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
				SlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
				//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(spinhandle);

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