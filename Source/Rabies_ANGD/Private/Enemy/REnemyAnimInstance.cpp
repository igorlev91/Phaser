// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/REnemyAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/Character.h"
#include "Enemy/REnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Character/RCharacterBase.h"
#include "AI/REnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"

bool UREnemyAnimInstance::ShouldDoUpperBody() const
{
	return (IsMoving() || IsJumping());
}

void UREnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
		PrevRot = OwnerCharacter->GetActorRotation();
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
		if (OwnerASC)
		{
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &UREnemyAnimInstance::ScopingTagChanged);
		}
	}

	if (StartMontage)
	{
		Montage_Play(StartMontage);
	}
}

void UREnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (OwnerCharacter && OwnerMovementComp)
	{
		Speed = OwnerCharacter->GetVelocity().Length();

		bIsJumping = OwnerMovementComp->IsFalling();

		FRotator characterRot = OwnerCharacter->GetActorRotation();
		FRotator lookRot = OwnerCharacter->GetViewRotation();

		LookOffset = (lookRot - characterRot).GetNormalized();

		FRotator RotDelta = (characterRot - PrevRot).GetNormalized();
		PrevRot = characterRot;

		YawSpeed = FMath::FInterpTo(YawSpeed, RotDelta.Yaw / DeltaSeconds, DeltaSeconds, 10.f);

		FVector Velocity = OwnerCharacter->GetVelocity();

		//  This is for getting the target
		ARCharacterBase* enemyCharacter = Cast<ARCharacterBase>(TryGetPawnOwner());
		if (enemyCharacter->GetTarget() == nullptr)
		{
			TargetLocation = FVector(0, 0, 0);
		}
		else
		{
			TargetLocation = (bIsScoping) ? enemyCharacter->GetTarget()->GetActorLocation() : TargetLocation;
		}

		FVector LookDir = lookRot.Vector();
		LookDir.Z = 0;
		LookDir.Normalize();
		FwdSpeed = Velocity.Dot(LookDir);
		RightSpeed = -Velocity.Dot(LookDir.Cross(FVector::UpVector));
	}
}

void UREnemyAnimInstance::ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsScoping = NewStackCount != 0;
}
