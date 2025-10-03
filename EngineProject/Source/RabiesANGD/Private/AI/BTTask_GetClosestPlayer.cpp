// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetClosestPlayer.h"

#include "AI/PatrollingComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TargetPoint.h"
#include "Framework/EOSActionGameState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"
#include "Framework/EOSPlayerState.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameStateBase.h"
#include "AI/REnemyAIController.h"
#include "Enemy/REnemyBase.h"
#include "GameFramework/Character.h"

#include "Character/RCharacterBase.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

EBTNodeResult::Type UBTTask_GetClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
		return EBTNodeResult::Failed;

	APlayerState* closestPlayer = nullptr;
	float closestDistance = TNumericLimits<float>::Max();
	for (APlayerState* playerState : GameState->PlayerArray)
	{
		if (playerState == nullptr)
			return EBTNodeResult::Failed;

		ARCharacterBase* character = Cast<ARCharacterBase>(playerState->GetPawn());
		if (character == nullptr)
			continue;

		if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) || character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetInvisTag()))
			continue;

		float distance = FVector::Dist(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), playerState->GetPawn()->GetActorLocation());
		if (distance < closestDistance)
		{
			closestDistance = distance;
			closestPlayer = playerState;
		}
	}

	if (closestPlayer == nullptr)
		return EBTNodeResult::Failed;

	AActor* chosenPlayer = Cast<AActor>(closestPlayer->GetPawn());
	if (chosenPlayer == nullptr)
		return EBTNodeResult::Failed;

	UAIPerceptionComponent* PerceptionComponent = AIC->GetPerceptionComponent();
	if (!PerceptionComponent)
		return EBTNodeResult::Failed;

	AREnemyBase* enemy = Cast<AREnemyBase>(AIC->GetPawn());
	if (enemy == nullptr)
		return EBTNodeResult::Failed;
		
	AIC->GetBlackboardComponent()->SetValueAsObject(TargetBlackboardKeyName, chosenPlayer);
	enemy->UpdateAITarget(chosenPlayer);
	
	FAIStimulus Stimulus;
	Stimulus.Tag = FName("Sight"); // Set the tag for the type of stimulus
	Stimulus.Strength = 1.0f; // Strength of the stimulus (e.g., full strength)
	Stimulus.StimulusLocation = chosenPlayer->GetActorLocation(); // Where the stimulus is coming from
	Stimulus.WasSuccessfullySensed(); // Whether it was successfully sensed
	Cast<AREnemyAIController>(AIC)->bAutoSense = true;
	//Cast<AREnemyAIController>(AIC)->TargetPerceptionUpdated(chosenPlayer, Stimulus);


	return EBTNodeResult::Succeeded;
}
