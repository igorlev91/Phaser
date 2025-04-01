// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BrawlerCombo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_BrawlerCombo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;
	if (CachedOwnerComp == nullptr)
		return EBTNodeResult::Failed;

	punchCount = 3;

	OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (OwningPawn)
	{
		punchCount--;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningPawn, URAbilityGenericTags::GetBasicAttackActivationTag(), FGameplayEventData());
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UBTTask_BrawlerCombo::DoAttack, 0.7f, false);

	return EBTNodeResult::InProgress;
}

void UBTTask_BrawlerCombo::DoAttack()
{
	if (OwningPawn && punchCount > 0)
	{
		punchCount--;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningPawn, URAbilityGenericTags::GetBasicAttackActivationTag(), FGameplayEventData());
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UBTTask_BrawlerCombo::DoAttack, 0.7f, false);
		return;
	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
