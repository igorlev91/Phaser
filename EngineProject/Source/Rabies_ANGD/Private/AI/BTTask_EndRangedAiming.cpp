// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EndRangedAiming.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_EndRangedAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		if (APawn* OwningPawn = AIC->GetPawn())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningPawn, URAbilityGenericTags::GetEndAimingTag(), FGameplayEventData());
		}
	}

	return EBTNodeResult::Succeeded;
}
