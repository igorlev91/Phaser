#include "GameplayAbilities/GA_Scoping.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Net/UnrealNetwork.h"
#include "Player/RPlayerController.h"
#include "Enemy/REnemyBase.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

UGA_Scoping::UGA_Scoping()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());

	CancelAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackActivationTag());
	CancelAbilitiesWithTag.AddTag(URAbilityGenericTags::GetSpecialAttackAimingTag());

	CancelAbilitiesWithTag.AddTag(URAbilityGenericTags::GetUltimateAttackActivationTag());
	CancelAbilitiesWithTag.AddTag(URAbilityGenericTags::GetUltimateAttackAimingTag());
}

void UGA_Scoping::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("Ending scope no authority"));
		K2_EndAbility();
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitStopEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, URAbilityGenericTags::GetEndScopingTag());
	WaitStopEvent->EventReceived.AddDynamic(this, &UGA_Scoping::StopScoping);
	WaitStopEvent->ReadyForActivation();

	if (K2_HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(ScopeSlowdownClass, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ScopeSlowDownEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
		//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
	}

	if (bSniperMode)
	{
		Player = Cast<ARPlayerBase>(GetOwningActorFromActorInfo());
		ScopingHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_Scoping::HoldingScope));
	}
}

void UGA_Scoping::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	for (ARCharacterBase* character : VisibleCharacters)
	{
		if (character == nullptr)
			continue;
		character->HideWeakpointUI(Player);
	}
	VisibleCharacters.Empty();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveActiveGameplayEffect(ScopeSlowDownEffectHandle);
	}
}

void UGA_Scoping::StopScoping(FGameplayEventData Payload)
{
	K2_EndAbility();
}

void UGA_Scoping::HoldingScope()
{
	FVector viewLoc;
	FRotator viewRot;

	Player->playerController->GetPlayerViewPoint(viewLoc, viewRot);

	// Find all actors in the world
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(Player->playerController->GetWorld(), ARCharacterBase::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		ARCharacterBase* character = Cast<ARCharacterBase>(Actor);

		if (!character || character == Player)
			continue;

		FVector DirectionToActor = (character->GetActorLocation() - viewLoc).GetSafeNormal();
		float DotProduct = FVector::DotProduct(viewRot.Vector(), DirectionToActor);
		float CosThreshold = FMath::Cos(FMath::DegreesToRadians(15.0f));

		if (DotProduct >= CosThreshold) // Actor is within the FOV angle
		{
			VisibleCharacters.Add(character);
			character->SetAndShowWeakpointUI(Player);
		}
		else
		{
			if (VisibleCharacters.Contains(character))
			{
				VisibleCharacters.Remove(character);
				character->HideWeakpointUI(Player);
			}
		}
	}

	ScopingHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UGA_Scoping::HoldingScope));
}
