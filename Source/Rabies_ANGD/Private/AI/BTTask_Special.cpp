// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Special.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_Special::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (APawn* OwningPawn = AIC->GetPawn())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningPawn, URAbilityGenericTags::GetSpecialAttackActivationTag(), FGameplayEventData());
		}
	}

	return EBTNodeResult::Succeeded;
}
