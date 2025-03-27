// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GA_SpiderDroneAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"


#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "AI/REnemyAIController.h"

#include "Framework/EOSActionGameState.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_SpiderDroneAttack::UGA_SpiderDroneAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_SpiderDroneAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Error, TEXT("Set up attack"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending Attack no commitment"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}


	UAbilityTask_WaitGameplayEvent* WaitForActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetBasicAttackActivationTag());
	WaitForActivation->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::TryCommitAttack);
	WaitForActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* AimingActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetStartAimingTag());
	AimingActivation->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::StartAiming);
	AimingActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* EndAimingActivation = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndAimingTag());
	EndAimingActivation->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::EndAiming);
	EndAimingActivation->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetGenericTargetAquiredTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_SpiderDroneAttack::HandleDamage);
	WaitForDamage->ReadyForActivation();

	Character = Cast<AREnemyBase>(GetOwningActorFromActorInfo());

	if (Character)
	{
		ClientHitScanHandle = Character->ClientHitScan.AddLambda([this](AActor* hitActor, FVector startPos, FVector endPos)
			{
				RecieveAttackHitscan(hitActor, startPos, endPos);
			});
	}
}

void UGA_SpiderDroneAttack::RecieveAttackHitscan(AActor* hitActor, FVector startPos, FVector endPos)
{
	if (K2_HasAuthority())
	{
		if (hitActor == nullptr) return;
		if (hitActor != Character)
		{
			FGameplayEventData Payload = FGameplayEventData();

			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(hitActor);

			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
			SignalDamageStimuliEvent(Payload.TargetData);
		}
	}
}

void UGA_SpiderDroneAttack::TryCommitAttack(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		Character->Hitscan(8000, nullptr);
	}
}

void UGA_SpiderDroneAttack::HandleDamage(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(AttackDamage, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
		SignalDamageStimuliEvent(Payload.TargetData);

	}
}

void UGA_SpiderDroneAttack::StartAiming(FGameplayEventData Payload)
{
	Character->UpdateAimingTagChange(true);
	

	/*if (K2_HasAuthority())
	{
		AREnemyAIController* aiController = Cast<AREnemyAIController>(Character->GetInstigatorController());
		if (aiController)
		{
			UBlackboardComponent* blackboardComp = aiController->GetBlackboardComponent();
			if (blackboardComp)
			{
				UObject* targetObj = blackboardComp->GetValueAsObject(TargetBlackboardKeyName);
				TargetActor = Cast<AActor>(targetObj);
			}
		}
	}*/
}

void UGA_SpiderDroneAttack::EndAiming(FGameplayEventData Payload)
{
	Character->UpdateAimingTagChange(false);

	//if (Character)
	//{
		//character->ServerPlayAnimMontage(AttackAnim);
	//}
}