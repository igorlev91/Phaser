// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_PlayAnimMontage.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RCharacterBase.h"

UBTTask_PlayAnimMontage::UBTTask_PlayAnimMontage()
{
	bNotifyTick = true; // needs to be ticking
}

EBTNodeResult::Type UBTTask_PlayAnimMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARCharacterBase* character = OwnerComp.GetAIOwner()->GetPawn<ARCharacterBase>();
	if (character)
	{
		float animLength = character->PlayAnimMontage(MontageToPlay);
		character->ClientPlayAnimMontage(MontageToPlay);
		PlayMontageNodeData* data = GetSpecialNodeMemory<PlayMontageNodeData>(NodeMemory);

		if (data)
		{
			data->MontagePlayed = MontageToPlay;
			data->MontageTimeLeft = animLength;
		}

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_PlayAnimMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	PlayMontageNodeData* data = GetSpecialNodeMemory<PlayMontageNodeData>(NodeMemory);
	if (data)
	{
		data->MontageTimeLeft -= DeltaSeconds;
		if (data->MontageTimeLeft <= 0)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

uint16 UBTTask_PlayAnimMontage::GetSpecialMemorySize() const
{
	return sizeof(PlayMontageNodeData);
}
