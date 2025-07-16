// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/REnemyBase.h"

#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"
#include "Widgets/WeakpointUI.h"
#include "Actors/PingActor.h"
#include "Actors/DamagePopupActor.h"

#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Framework/EOSActionGameState.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/RGameMode.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "TimerManager.h"
#include "Engine/World.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Abilities/GameplayAbility.h"
#include "Enemy/GA_EnemyMeleeAttack.h"

AREnemyBase::AREnemyBase()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AREnemyBase::HitDetected);

	AIPerceptionSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AI Perception Souce Comp");
	AIPerceptionSourceComp->RegisterForSense(UAISense_Sight::StaticClass());

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	GetMesh()->SetGenerateOverlapEvents(true);
}

void AREnemyBase::BeginPlay()
{
	Super::BeginPlay();

	OnDamageRecieved.AddUObject(this, &AREnemyBase::RecievedDamage);
}

void AREnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AREnemyBase::SetCollisionStatus(bool status)
{
	if (status) // is true
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else // is false
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AREnemyBase::RecievedDamage(float damage)
{
	if (DamagePopupActorClass != nullptr && HasAuthority())
	{

		FActorSpawnParameters SpawnParams;
		FVector spawnPos = GetActorLocation();
		ADamagePopupActor* newActor = GetWorld()->SpawnActor<ADamagePopupActor>(DamagePopupActorClass, spawnPos, FRotator::ZeroRotator, SpawnParams);
		newActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		int x = FMath::RandRange(-200, 200);
		int z = FMath::RandRange(20, 100);
		FVector randomAnimOffset = GetActorLocation() + FVector(x, 0.0f, z);

		newActor->SetText(damage, randomAnimOffset);
	}
}

void AREnemyBase::Multicast_SetRollerVisibility_Implementation(bool bShowRoller, bool bShowMain, float blendShape)
{
	TArray<USkeletalMeshComponent*> SkeletalComponents;
	GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	USkeletalMeshComponent* RollerSkeletalMesh = nullptr;

	for (USkeletalMeshComponent* Skeletal : SkeletalComponents)
	{
		if (Skeletal)
		{
			if (Skeletal->GetName() == TEXT("Roll"))
			{
				RollerSkeletalMesh = Skeletal;
			}
		}
	}

	if (RollerSkeletalMesh == nullptr)
		return;

	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AREnemyBase::SetRollerVisibilityDelay, bShowRoller, bShowMain, RollerSkeletalMesh);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);

	GetMesh()->SetMorphTarget(FName("ArmadilloDrone_geo_Blendhshape"), blendShape); // Full strength
}

void AREnemyBase::SetRollerVisibilityDelay(bool bShowRoller, bool bShowMain, USkeletalMeshComponent* roller)
{
	if (roller)
	{
		roller->SetVisibility(bShowRoller);
	}

	if (GetMesh())
	{
		GetMesh()->SetVisibility(bShowMain);
	}
}

void AREnemyBase::InitLevel_Implementation(int level)
{
	if (HasAuthority())
	{
		AILevel = level;
		ServerPlayAnimMontage(ReviveMontage);
		GetWorld()->GetTimerManager().SetTimer(LevelHandle, this, &AREnemyBase::CommitLevel, 0.1f, false);
	}
}

void AREnemyBase::CommitLevel_Implementation()
{
	OnDeadStatusChanged.AddUObject(this, &AREnemyBase::DeadStatusUpdated);

	LevelUpUpgrade(AILevel, true);
}

void AREnemyBase::UpdateAimingTagChange_Implementation(bool state)
{
	if (state)
	{
		GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetScopingTag());
	}
	else
	{
		GetAbilitySystemComponent()->RemoveLooseGameplayTag(URAbilityGenericTags::GetScopingTag());
	}
}

void AREnemyBase::HitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAISense_Touch::ReportTouchEvent(this, OtherActor, this, GetActorLocation());
}

void AREnemyBase::DeadStatusUpdated(bool bIsDead)
{
	//UE_LOG(LogTemp, Error, TEXT("Dead"));

	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		return;

	GetAbilitySystemComponent()->AddLooseGameplayTag(URAbilityGenericTags::GetDeadTag());

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetDeadTag(), eventData);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (AIPerceptionSourceComp) 
	{
		AIPerceptionSourceComp->UnregisterFromPerceptionSystem();
	}

	if (bIsDead)
	{
		if (GravityFallClass != nullptr)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			FGameplayEffectContextHandle contextHandle = GetAbilitySystemComponent()->MakeEffectContext();
			FGameplayEffectSpecHandle effectSpechandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GravityFallClass, 1.0f, contextHandle);

			FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
			if (spec)
			{

				GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
			}
		}

		if (DeadlockSmokeEffect != nullptr)
		{
			AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (gameState)
			{
				gameState->Multicast_RequestSpawnVFXOnCharacter(DeadlockSmokeEffect, this, GetActorLocation(), GetActorForwardVector(), 0);
			}
		}

		DropItemServerDeathRequest();
		PlayAnimMontage(DeathMontage);
		float Delay = FMath::FRandRange(-0.2f, 1.0f);
		float finalDeathTimer = deathTimer - Delay;
		GetWorld()->GetTimerManager().SetTimer(DeathHandle, this, &AREnemyBase::DelayServerDeathRequest, deathTimer, false);


		if (DeathSound != nullptr && HasAuthority())
		{
			AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
			if (GameState)
			{
				FVector Location = GetActorLocation();
				FRotator Rotation = FRotator::ZeroRotator;
				USoundAttenuation* Attenuation = nullptr;

				GameState->Multicast_RequestPlayAudio(DeathSound, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
			}
		}
	}
}

void AREnemyBase::DelayServerDeathRequest()
{
	if (HasAuthority())
	{
		AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
		if (gameState == GetOwner())
		{
			if (DeadlockSmokeEffect != nullptr)
			{
				gameState->Multicast_RequestSpawnVFX(DeadlockExplosionEffect, GetActorLocation(), GetActorForwardVector(), 0);
			}

			gameState->SelectEnemy(this, bIsDeadlock, bIsDeadlockComponent);
		}
	}
}

void AREnemyBase::DropItemServerDeathRequest()
{
	if (HasAuthority())
	{
		AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
		if (gameState == GetOwner())
		{
			gameState->SpawnDeathItem(this, itemDropChance, itemsToDrop);
		}
	}
}

void AREnemyBase::UpdateEnemyDeath_Implementation()
{
	Destroy();
}
