// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RCharacterBase.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Widgets/HealthBar.h"

#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"

// Sets default values
ARCharacterBase::ARCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AbilitySystemComponent = CreateDefaultSubobject<URAbilitySystemComponent>("Ability System Component");
	//AbilitySystemComponent->SetIsReplicated(true); // replicate means it is synced with the server.
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//AttributeSet = CreateDefaultSubobject<URAttributeSet>("Attribute Set");

	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &ARCharacterBase::HealthUpdated);
	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ARCharacterBase::MaxHealthUpdated);

	//AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetDeadTag()).AddUObject(this, &ARCharacterBase::DeathTagChanged);
	//AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &ARCharacterBase::ScopingTagChanged);

	//StatusWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	//StatusWidgetComp->SetupAttachment(GetRootComponent());

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARCharacterBase::HitDetected);

	//AIPerceptionSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AI Perception Source Comp");
	//AIPerceptionSourceComp->RegisterForSense(UAISense_Sight::StaticClass());
}