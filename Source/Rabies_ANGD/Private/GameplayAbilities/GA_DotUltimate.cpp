// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotUltimate.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/Dot/RDot_SpecialProj.h"
#include "Framework/EOSActionGameState.h"
#include "Enemy/REnemyBase.h"
#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"


#include "Kismet/KismetSystemLibrary.h"
#include "Player/Dot/RTargetActor_DotUltimate.h"
#include "Targeting/RTargetActor_DotSpecial.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


#include "Player/RPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_DotUltimate::UGA_DotUltimate()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.ultimate.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	/*FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);*/
}

void UGA_DotUltimate::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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


	UAbilityTask_WaitGameplayEvent* sendOffAttack = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateAttackActivationTag());
	sendOffAttack->EventReceived.AddDynamic(this, &UGA_DotUltimate::SendOffAttack);
	sendOffAttack->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* sendOfFinalfAttack = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetUltimateStrengthTag());
	sendOfFinalfAttack->EventReceived.AddDynamic(this, &UGA_DotUltimate::SendOffFinalAttack);
	sendOfFinalfAttack->ReadyForActivation();

	playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? TargettingMontageAir : TargettingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* waitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetActorClass);
	waitTargetDataTask->ValidData.AddDynamic(this, &UGA_DotUltimate::TargetAquired);
	waitTargetDataTask->Cancelled.AddDynamic(this, &UGA_DotUltimate::TargetCancelled);
	waitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataTask->BeginSpawningActor(this, targetActorClass, spawnedTargetActor);
	ARTargetActor_DotSpecial* dotPickActor = Cast<ARTargetActor_DotSpecial>(spawnedTargetActor);
	if (dotPickActor)
	{
		dotPickActor->SetOwningPlayerControler(Player->playerController);
		dotPickActor->SetTargettingRadus(500.0f);
		dotPickActor->SetTargettingRange(5000.0f);
	}
	waitTargetDataTask->FinishSpawningActor(this, dotPickActor);

	if (Player)
	{
		ClientHitScanHandle = Player->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos, bIsCrit);
			});
	}
}

void UGA_DotUltimate::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ClientHitScanHandle.IsValid() && Player)
	{
		StopDurationAudioEffect();
		Player->ClientHitScan.Remove(ClientHitScanHandle);
	}
}

void UGA_DotUltimate::TargetAquired(const FGameplayAbilityTargetDataHandle& Data)
{
	if (playTargettingMontageTask)
	{
		playTargettingMontageTask->OnBlendOut.RemoveAll(this);
		playTargettingMontageTask->OnInterrupted.RemoveAll(this);
		playTargettingMontageTask->OnCompleted.RemoveAll(this);
		playTargettingMontageTask->OnCancelled.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (!K2_CommitAbility())
		{
			K2_EndAbility();
			return;
		}
	}

	UAbilityTask_PlayMontageAndWait* playFinishMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? CastingMontageAir : CastingMontage);
	playFinishMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnCompleted.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->OnCancelled.AddDynamic(this, &UGA_DotUltimate::K2_EndAbility);
	playFinishMontageTask->ReadyForActivation();

	//TargetActorDotUltimate = GetWorld()->SpawnActor<ARTargetActor_DotUltimate>(UGA_DotUltimate::StaticClass());

	UAbilityTask_WaitTargetData* waitTargetDataUltimateTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, targetUltimateActorClass);
	waitTargetDataUltimateTask->ReadyForActivation();

	AGameplayAbilityTargetActor* spawnedTargetActor;
	waitTargetDataUltimateTask->BeginSpawningActor(this, targetUltimateActorClass, spawnedTargetActor);

	TargetActorDotUltimate = Cast<ARTargetActor_DotUltimate>(spawnedTargetActor);
	TargetActorDotUltimate->CylinderMesh->SetVisibility(false, true);
	waitTargetDataUltimateTask->FinishSpawningActor(this, TargetActorDotUltimate);

	GetWorld()->GetTimerManager().SetTimer(DamageTimer, this, &UGA_DotUltimate::CheckDamage, 0.05f, true);

	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &UGA_DotUltimate::StopAttacking, 5.5f, false);
}

void UGA_DotUltimate::TargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
}

void UGA_DotUltimate::CheckDamage()
{
	if (Player == nullptr)
		return;

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Capsule = FCollisionShape::MakeCapsule(CylinderRadius, CylinderHeight / 2);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player); // Ignore self
	QueryParams.AddIgnoredActor(TargetActorDotUltimate);


	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, CylinderLocation, FQuat(CylinderRotation), ECC_Pawn, Capsule, QueryParams);
	//DrawDebugCapsule(GetWorld(), CylinderLocation, CylinderHeight / 2, CylinderRadius, FQuat(CylinderRotation), FColor::Blue, false, 0.1f);

	for (const FOverlapResult& result : OverlappingResults)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			FGameplayEffectSpecHandle specHandle = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(CurrentDamage, 1.0f, Player->GetAbilitySystemComponent()->MakeEffectContext());

			FGameplayEffectSpec* spec = specHandle.Data.Get();
			if (spec)
			{
				enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
				Player->DealtDamage(enemy);
			}
		}
	}
}

void UGA_DotUltimate::StopAttacking()
{
	GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
	TargetActorDotUltimate->CylinderMesh->SetVisibility(false, true);

	FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(MeleePushClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), -1000.0f);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);
}

void UGA_DotUltimate::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos, bool bIsCrit)
{
	if (hitActor == nullptr) return;

	CurrentScale = FMath::Lerp(CurrentScale, CylinderScale, 20 * GetWorld()->GetDeltaSeconds());

	FVector adjustedEnd = endPos + ((endPos - startPos) * 0.5f);

	FVector Direction = (adjustedEnd - startPos);
	CylinderLocation = (startPos + (adjustedEnd)) * 0.5f;

	float Length = Direction.Size();
	CylinderRotation = UKismetMathLibrary::MakeRotFromZ((endPos - startPos));

	CylinderHeight = Length;

	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Server_RequestSpawnDotLaserMarks(HitEffectLaserBeam, endPos, Direction, ScorchSize);
	}

	if (TargetActorDotUltimate)
	{
		TargetActorDotUltimate->SetActorLocation(CylinderLocation);
		TargetActorDotUltimate->SetActorRotation(CylinderRotation);

		TargetActorDotUltimate->CylinderMesh->SetVisibility(true, true);

		FVector Scale = FVector(CurrentScale, CurrentScale, Length / 100.0f);

		TargetActorDotUltimate->CylinderMesh->SetRelativeScale3D(Scale);
	}
}

void UGA_DotUltimate::SendOffAttack(FGameplayEventData Payload)
{
	CylinderRadius = 37.5f;
	CylinderScale = 0.75f;
	CurrentDamage = AttackDamage;
	ScorchSize = 120;

	if (K2_HasAuthority())
	{
		if (Player)
		{

			Player->Hitscan(40000, Player->GetPlayerBaseState());

			StartDurationAudioEffect();
		}
	}
}

void UGA_DotUltimate::SendOffFinalAttack(FGameplayEventData Payload)
{
	CylinderRadius = 75.0f;
	CylinderScale = 1.5f;
	CurrentDamage = BigAttackDamage;
	ScorchSize = 250;

	if (K2_HasAuthority())
	{
		if (Player)
		{

			Player->Hitscan(40000, Player->GetPlayerBaseState());

			//StartDurationAudioEffect();
		}
	}
}