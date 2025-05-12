// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_DotMelee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Enemy/REnemyBase.h"
#include "Player/RPlayerBase.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GA_DotMelee.h"

UGA_DotMelee::UGA_DotMelee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetMeleeAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_DotMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	//float currentGravity = Player->GetCharacterMovement()->GravityScale;

	UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Player->IsFlying() ? FlyingAttackAnim : MeleeAttackAnim);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_DotMelee::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	if (Player->IsFlying())
	{
		bDealDamage = true;

		if (K2_HasAuthority())
		{
			if (AbilitySoundEfx.Num() > 0)
			{
				Player->PlayVoiceLine(AbilitySoundEfx, 60);
			}
		}

		FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(MeleePushClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), 0.0f);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);

		UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
		WaitForDamage->EventReceived.AddDynamic(this, &UGA_DotMelee::HandleDamage);
		WaitForDamage->ReadyForActivation();

		ApplyEffect(-4.0f);

		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, this, &UGA_DotMelee::DotSuperZoom, 0.5f, false);
		GetWorld()->GetTimerManager().SetTimer(FallTimerHandle, this, &UGA_DotMelee::DotFall, 0.3f, false);
		GetWorld()->GetTimerManager().SetTimer(RiseTimerHandle, this, &UGA_DotMelee::DotRise, 0.9f, false);

		TriggerAudioCueAir();
	}
	else
	{
		bDealDamage = false;

		if (K2_HasAuthority())
		{
			if (AbilitySoundEfx.Num() > 0)
			{
				Player->PlayVoiceLine(GroundedAbilitySoundEfx, 60);
			}
		}

		UAbilityTask_WaitGameplayEvent* WaitForPush = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetExpTag());
		WaitForPush->EventReceived.AddDynamic(this, &UGA_DotMelee::HandleEnemyPush);
		WaitForPush->ReadyForActivation();

		TriggerAudioCue();
	}

}

void UGA_DotMelee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
	}
}

void UGA_DotMelee::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority() && bDealDamage)
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		for (int32 i = 0; i < Payload.TargetData.Num(); ++i)
		{
			const FGameplayAbilityTargetData* Data = Payload.TargetData.Get(i);
			if (Data)
			{
				const TArray<TWeakObjectPtr<AActor>> Actors = Data->GetActors();
				for (const TWeakObjectPtr<AActor>& WeakActorPtr : Actors)
				{
					if (WeakActorPtr.IsValid())
					{
						ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(WeakActorPtr.Get());
						if (hitCharacter)
						{
							/*AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
							if (gameState)
							{
								AREnemyBase* enemy = Cast<AREnemyBase>(hitCharacter);
								if(enemy)
									gameState->Multicast_RobotGiblets(enemy->GetActorLocation(), enemy->GetActorUpVector(), enemy->GibletCount);
							}*/

							Player->HitMeleeAttack(hitCharacter);
						}
					}
				}
			}
		}
	}
}

void UGA_DotMelee::HandleEnemyPush(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		bool bFound = false;
		float strength =  Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackStrengthAttribute(), bFound);

		if (bFound == false)
			return;

		for (int32 i = 0; i < Payload.TargetData.Num(); ++i)
		{
			const FGameplayAbilityTargetData* Data = Payload.TargetData.Get(i);
			if (Data)
			{
				const TArray<TWeakObjectPtr<AActor>> Actors = Data->GetActors();
				for (const TWeakObjectPtr<AActor>& WeakActorPtr : Actors)
				{
					if (WeakActorPtr.IsValid())
					{
						ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(WeakActorPtr.Get());
						if (hitCharacter)
						{
							FVector LaunchVelocity = Player->GetActorForwardVector() * (1500.f + (strength * 60.0));  // Adjust strength as needed
							LaunchVelocity.Z += (400.0f + (strength * 30));
							hitCharacter->LaunchBozo(LaunchVelocity);
							SignalDamageStimuliEvent(Payload.TargetData);
						}
					}
				}
			}
		}
	}
}

void UGA_DotMelee::DotSuperZoom()
{

	// FOR TESTING SELF DAMAGE
	//FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	//ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);

	FGameplayEffectSpecHandle pushSpec = MakeOutgoingGameplayEffectSpec(MeleePushClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	pushSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), 1000.0f);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, pushSpec);
}

void UGA_DotMelee::DotFall()
{
	ApplyEffect(3.5f);
}

void UGA_DotMelee::DotRise()
{
	ApplyEffect(-7.0f);
}

void UGA_DotMelee::ApplyEffect(float value)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(GravityFallEffectHandle);
	}

	FGameplayEffectSpecHandle fallSpec = MakeOutgoingGameplayEffectSpec(GravityClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	fallSpec.Data.Get()->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), value);

	GravityFallEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, fallSpec);
}

void UGA_DotMelee::TriggerAudioCueAir()
{
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(AudioCueTagAir, CueParams);
}
