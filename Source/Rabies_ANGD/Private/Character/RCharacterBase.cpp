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

	AbilitySystemComponent = CreateDefaultSubobject<URAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true); // replicate means it is synced with the server.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<URAttributeSet>("Attribute Set");

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &ARCharacterBase::HealthUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ARCharacterBase::MaxHealthUpdated);

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
}

void ARCharacterBase::SetupAbilitySystemComponent()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ARCharacterBase::InitAttributes()
{
	AbilitySystemComponent->ApplyInitialEffects();
}

void ARCharacterBase::InitAbilities()
{
	AbilitySystemComponent->GrantInitialAbilities();
}

void ARCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	InitStatusHUD();
}

// Called every frame
void ARCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupAbilitySystemComponent();
	InitAttributes();
	InitAbilities();

	if (NewController && !NewController->IsPlayerController())
	{
		SetupAbilitySystemComponent();
		InitAttributes();
		InitAbilities();
	}

	if (HasAuthority() && Controller && Controller->IsPlayerController())
	{
		APlayerController* OwningPlayerController = Cast<APlayerController>(Controller);
		// Find the ID

		TeamId = FGenericTeamId(1);
	}
}

void ARCharacterBase::InitStatusHUD()
{
	HealthBar = Cast<UHealthBar>(HealthBarWidgetComp->GetUserWidgetObject());
	if (!HealthBar)
	{
		UE_LOG(LogTemp, Error, TEXT("%s can't spawn health has the wrong widget setup"), *GetName());
		return;
	}

	HealthBar->SetRenderScale(FVector2D{ 0.5f });

	UE_LOG(LogTemp, Error, TEXT("health is: %d / %d"), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	HealthBar->SetHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());

	if (IsLocallyControlled())
	{
		if (GetController() && GetController()->IsPlayerController())
			HealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

UAbilitySystemComponent* ARCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARCharacterBase::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (HealthBar)
		HealthBar->SetHealth(ChangeData.NewValue, AttributeSet->GetMaxHealth());

	if (HasAuthority())
	{
		if (ChangeData.NewValue >= AttributeSet->GetMaxHealth())
		{
			AbilitySystemComponent->AddLooseGameplayTag(URAbilityGenericTags::GetFullHealthTag());
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(URAbilityGenericTags::GetFullHealthTag());
		}
	}

	if (ChangeData.NewValue <= 0)
	{
		// die
	}
}

void ARCharacterBase::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (HealthBar)
		HealthBar->SetHealth(AttributeSet->GetHealth(), ChangeData.NewValue);
}

void ARCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARCharacterBase, TeamId, COND_None);
}