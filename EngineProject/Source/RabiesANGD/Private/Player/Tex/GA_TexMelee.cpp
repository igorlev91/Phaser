// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Tex/GA_TexMelee.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayTagsManager.h"

#include "Player/RPlayerBase.h"

#include "Framework/EOSActionGameState.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

UGA_TexMelee::UGA_TexMelee()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetMeleeAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_TexMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (Player == nullptr)
		return;

	bool bFound = false;
	float attackCooldown = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackCooldownReductionAttribute(), bFound);
	float attackSpeed = 1.0f;

	if (bFound == true)
	{
		attackSpeed = 3.0f - 2.0f * (attackCooldown - 0.1f) / (1.0f - 0.1f);
		//UE_LOG(LogTemp, Error, TEXT("Attack Speed: %f"), attackSpeed);
	}

	SetupWaitInputTask();

	UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MeleeAttackAnim, attackSpeed);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_TexMelee::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_TexMelee::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_TexMelee::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_TexMelee::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetComboChange(), nullptr, false, false);
	WaitComboEvent->EventReceived.AddDynamic(this, &UGA_TexMelee::HandleCombo);
	WaitComboEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboEvent2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitComboEvent2->EventReceived.AddDynamic(this, &UGA_TexMelee::TryCommitCombo);
	WaitComboEvent2->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_TexMelee::HandleDamage);
	WaitForDamage->ReadyForActivation();

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 60);
		}
	}

	TriggerAudioCue();
}

void UGA_TexMelee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetCooldownRefreshTag(), FGameplayEventData());
}

void UGA_TexMelee::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

		if (SpecialCooldownClass)
		{
			FGameplayEffectQuery Query;
			Query.EffectDefinition = SpecialCooldownClass;

			TArray<FActiveGameplayEffectHandle> ActiveEffects = Player->GetAbilitySystemComponent()->GetActiveEffects(Query);
			for (FActiveGameplayEffectHandle Handle : ActiveEffects)
			{
				const FActiveGameplayEffect* activeEffect = Player->GetAbilitySystemComponent()->GetActiveGameplayEffect(Handle);
				if (activeEffect)
				{
					float remainingTime = activeEffect->GetTimeRemaining(Player->GetAbilitySystemComponent()->GetWorld()->GetTimeSeconds());
					if (remainingTime <= 0)
						return;

					float NewTime = remainingTime - 10.0f;
					NewTime = FMath::Clamp(NewTime, 0.01f, NewTime);

					Player->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(Handle);

					// Reapply a new cooldown effect with adjusted duration
					if (NewTime >= 0)
					{
						FGameplayEffectSpecHandle NewCooldownSpec = Player->GetAbilitySystemComponent()->MakeOutgoingSpec(SpecialCooldownClass, 1, Player->GetAbilitySystemComponent()->MakeEffectContext());
						NewCooldownSpec.Data->SetDuration(NewTime, true);
						Player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*NewCooldownSpec.Data);
						//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetCooldownRefreshTag(), FGameplayEventData());
					}
				}
			}
		}

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
							Player->HitMeleeAttack(hitCharacter);
						}
					}
				}
			}
		}
	}
}

void UGA_TexMelee::HandleCombo(FGameplayEventData Payload)
{
	FGameplayTag comboTag = Payload.EventTag;
	if (comboTag == URAbilityGenericTags::GetComboEnd())
	{
		NextComboName = NAME_None;
		bComboComitted = false;
		return;
	}

	TArray<FName> comboNames;
	//UGameplayTagsManager::Get().SplitGameplayTagFName(comboTag, comboNames);
	NextComboName = "Combo2";//comboNames.Last(); // using takes and other things you can set up a bunch of dividers in the anim for the ability

}

void UGA_TexMelee::TryCommitCombo(FGameplayEventData Payload)
{
	if (bComboComitted)
	{
		return;
	}

	if (NextComboName == NAME_None)
	{
		return;
	}

	USkeletalMeshComponent* ownerMesh = GetOwningComponentFromActorInfo();
	if (!ownerMesh)
	{
		return;
	}

	UAnimInstance* ownerAnimInst = ownerMesh->GetAnimInstance();
	if (!ownerAnimInst)
		return;

	ownerAnimInst->Montage_SetNextSection(ownerAnimInst->Montage_GetCurrentSection(MeleeAttackAnim), NextComboName, MeleeAttackAnim);
	ownerAnimInst->Montage_JumpToSection(NextComboName, MeleeAttackAnim);
	bComboComitted = true;
	TriggerAudioCue();
}

void UGA_TexMelee::AbilityInputPressed(float timeWaited)
{
	SetupWaitInputTask();
	TryCommitCombo(FGameplayEventData());
}

void UGA_TexMelee::SetupWaitInputTask()
{
	UAbilityTask_WaitInputPress* waitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	waitInputPress->OnPress.AddDynamic(this, &UGA_TexMelee::AbilityInputPressed);
	waitInputPress->ReadyForActivation();
}
