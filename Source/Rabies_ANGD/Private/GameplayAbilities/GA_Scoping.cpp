#include "GameplayAbilities/GA_Scoping.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Scoping::UGA_Scoping()
{
	ActivationOwnedTags.AddTag(URAbilityGenericTags::GetScopingTag());
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
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec);
		//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpec, Payload.TargetData);
	}
}

void UGA_Scoping::StopScoping(FGameplayEventData Payload)
{
	URAbilitySystemComponent* AbilitySystem = Cast<URAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (AbilitySystem)
	{
		FGameplayEffectQuery Query;
		Query.EffectDefinition = ScopeSlowdownClass;

		const TArray<FActiveGameplayEffectHandle>& ActiveEffects = AbilitySystem->GetActiveEffects(Query);
		if (ActiveEffects.Num() > 0)
		{
			AbilitySystem->RemoveActiveGameplayEffect(ActiveEffects[0]);
		}
	}

	K2_EndAbility();
}
