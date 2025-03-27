// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RCharacterBase.h"
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

#include "Widgets/HealthBar.h"

#include "Framework/RAttackingBoxComponent.h"

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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &ARCharacterBase::MovementSpeedUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetGravityAttribute()).AddUObject(this, &ARCharacterBase::GravityUpdated);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &ARCharacterBase::ScopingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetFlyingTag()).AddUObject(this, &ARCharacterBase::FlyingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetTakeOffDelayTag()).AddUObject(this, &ARCharacterBase::TakeOffDelayTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetHoldingJump()).AddUObject(this, &ARCharacterBase::HoldingJumpTagChanged);

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());

	AttackingBoxComponent = CreateDefaultSubobject<URAttackingBoxComponent>("Attacking Box Component");
	AttackingBoxComponent->SetupAttachment(GetMesh());
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

	//SetupAbilitySystemComponent();
	//InitAttributes();
	//InitAbilities();

	if (NewController && NewController->IsPlayerController())
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

	if (AttributeSet)
	{
		//UE_LOG(LogTemp, Error, TEXT("health is: %d / %d"), AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
		HealthBar->SetHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
	}

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

void ARCharacterBase::PlayMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		if (GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
		}
	}
}

void ARCharacterBase::StartDeath()
{
	PlayMontage(DeathMontage);
	//AbilitySystemComponent->ApplyGameplayEffect(DeathEffect);
	AbilitySystemComponent->AddLooseGameplayTag(URAbilityGenericTags::GetDeadTag());
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnDeadStatusChanged.Broadcast(true);
}

void ARCharacterBase::DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	if (NewStackCount == 0) // for getting revived
	{
		StopAnimMontage(DeathMontage);
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		AbilitySystemComponent->ApplyFullStat();
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// change the AI perception
		OnDeadStatusChanged.Broadcast(false);
	}
}

void ARCharacterBase::ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsScoping = NewStackCount != 0;
	ScopingTagChanged(bIsScoping);
}

void ARCharacterBase::FlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsFlying = NewStackCount != 0;
	FlyingTagChanged(bIsFlying);
}

void ARCharacterBase::TakeOffDelayTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bTakeOffDelay = NewStackCount != 0;
	TakeOffDelayTagChanged(bTakeOffDelay);
}

void ARCharacterBase::HoldingJumpTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bHoldingJump = NewStackCount != 0;
	HoldingJumpTagChanged(bHoldingJump);
}

void ARCharacterBase::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

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
		StartDeath();
		if (HasAuthority() && ChangeData.GEModData)
		{

		}
	}
}

void ARCharacterBase::MaxHealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	if (HealthBar)
		HealthBar->SetHealth(AttributeSet->GetHealth(), ChangeData.NewValue);
}

void ARCharacterBase::MovementSpeedUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("Speed is: %f / and their new walk speed is: %f"), AttributeSet->GetMovementSpeed(), ChangeData.NewValue);
	GetCharacterMovement()->MaxWalkSpeed = ChangeData.NewValue;
}

void ARCharacterBase::GravityUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	GetCharacterMovement()->GravityScale = ChangeData.NewValue;
}

void ARCharacterBase::ServerPlayAnimMontage_Implementation(UAnimMontage* montage)
{
	PlayAnimMontage(montage);
}

void ARCharacterBase::ClientPlayAnimMontage_Implementation(UAnimMontage* montage)
{
	if (!HasAuthority())
	{
		PlayAnimMontage(montage);
	}
}

void ARCharacterBase::ClientStopAnimMontage_Implementation(UAnimMontage* montage)
{
	if (!HasAuthority())
	{
		StopAnimMontage(montage);
	}
}

void ARCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARCharacterBase, TeamId, COND_None);
}