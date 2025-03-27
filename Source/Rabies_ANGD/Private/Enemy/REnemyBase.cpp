// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/REnemyBase.h"

#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/RGameMode.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Abilities/GameplayAbility.h"
#include "Enemy/GA_EnemyMeleeAttack.h"

AREnemyBase::AREnemyBase()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AREnemyBase::HitDetected);

	AIPerceptionSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AI Perception Souce Comp");
	AIPerceptionSourceComp->RegisterForSense(UAISense_Sight::StaticClass());

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AREnemyBase::BeginPlay()
{
	Super::BeginPlay();
	OnDeadStatusChanged.AddUObject(this, &AREnemyBase::DeadStatusUpdated);
}

void AREnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AREnemyBase::HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAISense_Touch::ReportTouchEvent(this, OtherActor, this, GetActorLocation());
}

void AREnemyBase::DeadStatusUpdated(bool bIsDead)
{
	UE_LOG(LogTemp, Error, TEXT("Dead"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AIPerceptionSourceComp->UnregisterFromPerceptionSystem();
}
