// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotFlying.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Framework/EOSPlayerState.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"
#include "GameplayAbilities/RAttributeSet.h"


#include "Framework/EOSActionGameState.h"

#include "Components/CapsuleComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UGA_DotFlying::UGA_DotFlying()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetFlyingTag());
}

void UGA_DotFlying::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	bFlying = false;

	UAbilityTask_WaitGameplayEvent* EndTakeOffEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndTakeOffChargeTag());
	EndTakeOffEvent->EventReceived.AddDynamic(this, &UGA_DotFlying::StopTakeOff);
	EndTakeOffEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* GravityJumpEffectAdd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetApplyGravityJump());
	GravityJumpEffectAdd->EventReceived.AddDynamic(this, &UGA_DotFlying::ApplyGravityJump);
	GravityJumpEffectAdd->ReadyForActivation();

	GetWorld()->GetTimerManager().ClearTimer(SlowFallHandle);
	SlowFallHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::ProcessFlying));

	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
	CurrentHoldDuration = 0.0f;

	if (!Player->GetCharacterMovement() || Player->GetCharacterMovement()->IsFalling()) return;

	if (AirComboReset)
	{
		FGameplayEffectSpecHandle EffectSpec333 = MakeOutgoingGameplayEffectSpec(AirComboReset, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec333);
	}

	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->playerController->ChangeTakeOffState(true, CurrentHoldDuration);

	if (Player->GetPlayerBaseState())
	{
		Player->GetPlayerBaseState()->Server_ProcessDotFlyingStamina(CurrentHoldDuration);
	}

	TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
	
}

void UGA_DotFlying::TryPickUpTeammates()
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (bHasTeammatePickedup && YoinkedPlayer)
	{
		if (gameState)
		{
			gameState->Multicast_CenterOnDot(YoinkedPlayer, Player);
		}
		return;
	}

	if (Player->bWindingUp)
		return;

	FVector FootLocation = Player->GetMesh()->GetSocketLocation("grabPoint");
	float SphereRadius = 85.0f;

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	bool bHit = GetWorld()->OverlapMultiByObjectType(Overlaps,FootLocation,FQuat::Identity,ObjectQueryParams,FCollisionShape::MakeSphere(SphereRadius),QueryParams);

	if (bHit)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (OverlappedActor && OverlappedActor != Player)
			{
				ARPlayerBase* player = Cast<ARPlayerBase>(OverlappedActor);
				if (player)
				{
					if (bHasTeammatePickedup == false)
					{
						if (gameState)
						{
							gameState->Multicast_StickPlayerOnDot(player, Player, true);
						}

						YoinkedPlayer = player;
						bHasTeammatePickedup = true;
					}
				}
			}
		}
	}

	// Optional: draw debug sphere
	//DrawDebugSphere(GetWorld(), FootLocation, SphereRadius, 16, FColor::Green, false, 0.01f);
}


void UGA_DotFlying::StopFlying()
{
	K2_EndAbility();
}

void UGA_DotFlying::ProcessFlying()
{
	if (bFlying)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
		}

		if (Player->GetCharacterMovement()->IsFalling() == false && Player->IsTakeOffDelay() == false)
		{
			StopFlying();
		}

		if (Player->IsHoldingJump())
		{
			TryPickUpTeammates();

			if (CurrentGravityDuration >= 1)
			{
				FVector currentVelocity = Player->PlayerVelocity;
				if (currentVelocity.Z <= 0 && Player->IsMeleeAttacking() == false)
				{
					float fallValue = (CurrentHoldDuration > 0) ? currentVelocity.Z * 0.02f : (currentVelocity.Z * 0.001f) + 0.3f; // these are fall gravity values, bigger means slower fall
					float newGravity = fallValue; //FMath::Lerp(currentGravity, fallValue, 20 * GetWorld()->GetDeltaSeconds());

					FGameplayEffectSpecHandle fallSpec = MakeOutgoingGameplayEffectSpec(GravityFallClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
					fallSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), newGravity);

					GravityFallEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, fallSpec);
				}

				currentVelocity.Z = 0;

				bool bFound = false;
				float strength = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackStrengthAttribute(), bFound);

				if (bFound == false)
					return;

				float OrigMin = 10.0f;
				float OrigMax = 90.0f;
				float TargetMin = 0.07f;
				float TargetMax = 2.0f;

				strength = FMath::Clamp(strength, OrigMin, OrigMax);

				float ScaledStrength = ((OrigMax - strength) / (OrigMax - OrigMin)) * (TargetMax - TargetMin) + TargetMin;

				float velocityLength = currentVelocity.Length() * 0.001f;
				float velocityMultiplier = FMath::Clamp(velocityLength, 0.015f, 2.0f);
				float multiplier = velocityMultiplier * ScaledStrength * 0.1f;

				CurrentHoldDuration -= GetWorld()->GetDeltaSeconds() * multiplier;
				Player->playerController->ChangeTakeOffState(true, CurrentHoldDuration);

				if (Player->GetPlayerBaseState())
				{
					Player->GetPlayerBaseState()->Server_ProcessDotFlyingStamina(CurrentHoldDuration);
				}
			}
		}
		else
		{
			if (bHasTeammatePickedup && YoinkedPlayer)
			{
				AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
				if (gameState)
				{
					gameState->Multicast_StickPlayerOnDot(YoinkedPlayer, Player, false);
				}

				YoinkedPlayer = nullptr;
				bHasTeammatePickedup = false;
			}
		}

	}

	SlowFallHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::ProcessFlying));
}

void UGA_DotFlying::StopTakeOff(FGameplayEventData Payload)
{
	if (bFlying)
	{
		return;
	}

	K2_EndAbility();
}

void UGA_DotFlying::Hold(float timeRemaining)
{
	if (CurrentHoldDuration <= 1)
	{
		Player->bWindingUp = true;

		bool bFound = false;
		float movementSPeed = 1.0f;
		movementSPeed = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMovementSpeedAttribute(), bFound);

		if (bFound == true)
		{
			movementSPeed *= 0.01f;
		}

		CurrentHoldDuration += GetWorld()->GetDeltaSeconds() * movementSPeed;
		bool visible = (CurrentHoldDuration >= 1 / 8.0f) ? true : false;
		Player->playerController->ChangeTakeOffState(visible, CurrentHoldDuration);

		if (Player->GetPlayerBaseState())
		{
			Player->GetPlayerBaseState()->Server_ProcessDotFlyingStamina(CurrentHoldDuration);
		}

		TakeOffHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::Hold, CurrentHoldDuration));
	}
	else
	{
		StartDurationAudioEffect();

		bFlying = true;

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(FlyingSpeedClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		FlyingSpeedEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

		FGameplayEffectSpecHandle EffectSpec2 = MakeOutgoingGameplayEffectSpec(GravityJumpClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		GravityJumpEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec2);

		Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
		Player->ServerPlayAnimMontage(TakeOffMontage);

		GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);
		CurrentGravityDuration = 0;
		GravityHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::GravityJump, CurrentGravityDuration));
	}
}

void UGA_DotFlying::GravityJump(float timeRemaining)
{
	if (CurrentGravityDuration >= 0.6f)
		Player->bWindingUp = false;

	if (CurrentGravityDuration <= 1.0f)
	{
		CurrentGravityDuration += GetWorld()->GetDeltaSeconds();
		GravityHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_DotFlying::GravityJump, CurrentGravityDuration));
	}
	else
	{
		RemoveGravityJump();
	}
}

void UGA_DotFlying::ApplyGravityJump(FGameplayEventData Payload)
{
	Player->Jump();
}

void UGA_DotFlying::RemoveGravityJump()
{
	Player->GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());
}

void UGA_DotFlying::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(FlyingSpeedEffectHandle);
		ASC->RemoveActiveGameplayEffect(GravityJumpEffectHandle);
		ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
	}

	FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AirComboReset, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

	if (bHasTeammatePickedup && YoinkedPlayer)
	{
		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (gameState)
		{
			gameState->Multicast_StickPlayerOnDot(YoinkedPlayer, Player, false);
		}

		YoinkedPlayer = nullptr;
		bHasTeammatePickedup = false;
	}

	Player->playerController->ChangeTakeOffState(false, 0);

	if (Player->GetPlayerBaseState())
	{
		Player->GetPlayerBaseState()->Server_ProcessDotFlyingStamina(1);
	}

	GetWorld()->GetTimerManager().ClearTimer(TakeOffHandle);

	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetTakeOffDelayTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetFlyingTag());
	Player->GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetUnActionableTag());

	Player->ServerPlayAnimMontage(HardLandingMontage);
}