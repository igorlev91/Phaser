// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetRandomPlayer.h"

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

EBTNodeResult::Type UBTTask_GetRandomPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if(GameState == nullptr)
		return EBTNodeResult::Failed;

	float randomPlayer = FMath::RandRange(0, GameState->PlayerArray.Num() - 1);
	if(GameState->PlayerArray[randomPlayer] == nullptr)
		return EBTNodeResult::Failed;

	AActor* chosenPlayer = Cast<AActor>(GameState->PlayerArray[randomPlayer]->GetPawn());
	if(chosenPlayer == nullptr)
		return EBTNodeResult::Failed;

	ARCharacterBase* character = Cast<ARCharacterBase>(chosenPlayer);
	if (character == nullptr)
		return EBTNodeResult::Failed;

	if (character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) || character->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetInvisTag()))
		return EBTNodeResult::Failed;

	UAIPerceptionComponent* PerceptionComponent = AIC->GetPerceptionComponent();
	if (!PerceptionComponent)
		return EBTNodeResult::Failed;

	FAIStimulus Stimulus;
	Stimulus.Tag = FName("Sight"); // Set the tag for the type of stimulus
	Stimulus.Strength = 1.0f; // Strength of the stimulus (e.g., full strength)
	Stimulus.StimulusLocation = chosenPlayer->GetActorLocation(); // Where the stimulus is coming from
	Stimulus.WasSuccessfullySensed(); // Whether it was successfully sensed
	Cast<AREnemyAIController>(AIC)->bAutoSense = true;
	Cast<AREnemyAIController>(AIC)->TargetPerceptionUpdated(chosenPlayer, Stimulus);

	return EBTNodeResult::Succeeded;
}
