// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/REnemyAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/GA_AbilityBase.h"

#include "Player/RPlayerBase.h"

#include "Net/UnrealNetwork.h"

#include "Character/RCharacterBase.h"
#include "GameplayAbilities/GA_AbilityBase.h"

#include "Enemy/REnemyBase.h"

#include "Enemy/GA_EnemyMeleeAttack.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"

AREnemyAIController::AREnemyAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SightRadius = 1850.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>("Damage Config");
	DamageConfig->SetMaxAge(5.0f);

	TouchConfig = CreateDefaultSubobject<UAISenseConfig_Touch>("Touch Config");
	TouchConfig->SetMaxAge(5.0f);

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*DamageConfig);
	AIPerceptionComponent->ConfigureSense(*TouchConfig);
}

FGenericTeamId AREnemyAIController::GetGenericTeamId() const
{
	return FGenericTeamId(254);
}

void AREnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	SetGenericTeamId(254);

	if (BehaviorTree)
		RunBehaviorTree(BehaviorTree);

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AREnemyAIController::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AREnemyAIController::TargetForgotton);
}

void AREnemyAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (GetCharacter() && GetCharacter()->GetMesh())
	{
		OutLocation = GetCharacter()->GetMesh()->GetSocketLocation(AIVisionSocketName);
		OutRotation = GetCharacter()->GetMesh()->GetSocketRotation(AIVisionSocketName);
		OutRotation.Pitch = 0;
	}
	else
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	}
}

void AREnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ARCharacterBase* character = Cast<ARCharacterBase>(InPawn);
	if (character)
	{
		Enemy = Cast<AREnemyBase>(character);
		if (Enemy)
		{
			Enemy->OnDeadStatusChanged.AddUObject(this, &AREnemyAIController::PawnDeathStatusChanged);
		}
	}
}

void AREnemyAIController::TargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus)
{
	if (!GetBlackboardComponent()) return;

	ARPlayerBase* player = Cast<ARPlayerBase>(Target);
	if (!player)
	{
		return;
	}

	URAbilitySystemComponent* TargetASC = Cast<URAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target)); 
	if(TargetASC->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		return;

	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName))
		{
			player->OnDeadStatusChanged.AddUObject(this, &AREnemyAIController::PlayerDeadStatusUpdated);
			Enemy->UpdateAITarget(Target);
			GetBlackboardComponent()->SetValueAsObject(TargetBlackboardKeyName, Target);
		}
	}
	else
	{
		if (TargetASC)
		{
			auto Iterator = PerceptionComponent->GetPerceptualDataIterator();
			while (Iterator)
			{
				if (Iterator->Value.Target == Target)
				{
					for (FAIStimulus& Stimi : Iterator->Value.LastSensedStimuli)
					{
						Stimi.SetStimulusAge(TNumericLimits<float>::Max());
					}
				}
				++Iterator;
			}
		}
	}
}

void AREnemyAIController::TargetForgotton(AActor* Target)
{
	AActor* currentTarget = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName));
	if (currentTarget == Target)
	{
		ProcessForgottonTarget(Target);
	}
}

void AREnemyAIController::ProcessForgottonTarget(AActor* Target)
{
	TArray<AActor*> OtherTargets;
	PerceptionComponent->GetPerceivedHostileActors(OtherTargets);
	if (OtherTargets.Num() != 0)
	{
		Enemy->UpdateAITarget(OtherTargets[0]);
		GetBlackboardComponent()->SetValueAsObject(TargetBlackboardKeyName, OtherTargets[0]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Forgot player completely"));
		Enemy->UpdateAITarget(nullptr);
		GetBlackboardComponent()->ClearValue(TargetBlackboardKeyName);
	}
}

void AREnemyAIController::PlayerDeadStatusUpdated(bool bIsDead)
{
	if (bIsDead)
	{
		AActor* currentTarget = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName));
		ARPlayerBase* player = Cast<ARPlayerBase>(currentTarget);
		if (player)
		{
			PerceptionComponent->ForgetActor(currentTarget);
			ProcessForgottonTarget(currentTarget);
			player->OnDeadStatusChanged.RemoveAll(this);

		}
	}
}

void AREnemyAIController::PawnDeathStatusChanged(bool bIsDead)
{
	if (bIsDead)
	{
		GetBrainComponent()->StopLogic("Dead");
	}
	else
	{
		GetBrainComponent()->StartLogic();
	}
}
