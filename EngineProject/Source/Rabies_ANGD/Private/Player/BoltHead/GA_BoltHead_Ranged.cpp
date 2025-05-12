// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/GA_BoltHead_Ranged.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "Player/Chester/ChesterBallActor.h"
#include "Player/Chester/RChester_RangeProj.h"
#include "Player/RPlayerController.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/BoltHead/RBoltHead_RangedProj.h"

#include "Player/Dot/RDot_RangedRevUpCooldown.h"
#include "Player/Dot/RDot_RangedAttack_Cooldown.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Enemy/REnemyBase.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_BoltHead_Ranged::UGA_BoltHead_Ranged()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("ability.attack.activate"));
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetAttackingTag());

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = URAbilityGenericTags::GetBasicAttackActivationTag();
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_BoltHead_Ranged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
	if (Player == nullptr)
		return;

	/*UAbilityTask_PlayMontageAndWait* playTargettingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootingMontage);
	playTargettingMontageTask->OnBlendOut.AddDynamic(this, &UGA_BoltHead_Ranged::K2_EndAbility);
	playTargettingMontageTask->OnInterrupted.AddDynamic(this, &UGA_BoltHead_Ranged::K2_EndAbility);
	playTargettingMontageTask->OnCompleted.AddDynamic(this, &UGA_BoltHead_Ranged::K2_EndAbility);
	playTargettingMontageTask->OnCancelled.AddDynamic(this, &UGA_BoltHead_Ranged::K2_EndAbility);
	playTargettingMontageTask->ReadyForActivation();*/

	bool bFound = false;
	float attackCooldown = Player->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetRangedAttackCooldownReductionAttribute(), bFound);
	float attackSpeed = 1.0f;

	if (bFound == true)
	{
		attackSpeed = 2.8f - 2.0f * (attackCooldown - 0.1f) / (1.0f - 0.1f);
		//UE_LOG(LogTemp, Error, TEXT("Attack Speed: %f"), attackSpeed);
	}

	Player->ServerPlay_Torso_AnimMontage(ShootingMontage, attackSpeed);

	UAbilityTask_WaitGameplayEvent* WaitForDamage = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetRangedAttackRightActivationTag());
	WaitForDamage->EventReceived.AddDynamic(this, &UGA_BoltHead_Ranged::Fire);
	WaitForDamage->ReadyForActivation();
	TriggerAudioCue();

	if (K2_HasAuthority())
	{
		if (AbilitySoundEfx.Num() > 0)
		{
			Player->PlayVoiceLine(AbilitySoundEfx, 30);
		}
	}

	FTimerHandle endTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(endTimerHandle, this, &UGA_BoltHead_Ranged::ForceEnd, 0.2f, false);

}


void UGA_BoltHead_Ranged::Fire(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		//UE_LOG(LogTemp, Error, TEXT("Sending Attack"));
		FVector viewLoc;
		FRotator viewRot;

		Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);
		FVector spawnPos = Player->GetActorLocation();
		spawnPos += (viewRot.Vector()) * 50.0f;

		ARBoltHead_RangedProj* newProjectile = GetWorld()->SpawnActor<ARBoltHead_RangedProj>(BoltHeadProjectile, spawnPos, viewRot);
		newProjectile->Init(RangedDamage, Player);
		newProjectile->InitOwningCharacter(Player);
		newProjectile->SetOwner(Player);

		K2_EndAbility();
	}
}

void UGA_BoltHead_Ranged::ForceEnd()
{
	K2_EndAbility();
}


void UGA_BoltHead_Ranged::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}