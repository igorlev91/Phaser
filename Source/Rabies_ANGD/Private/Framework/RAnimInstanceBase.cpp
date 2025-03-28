// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RAnimInstanceBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/Character.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

bool URAnimInstanceBase::ShouldDoUpperBody() const
{
	return (IsMoving() || IsJumping() || GetIsAiming());
}

bool URAnimInstanceBase::ShouldDotBeFlapping() const
{
	return (GetFlying() && GetIsAiming());
}

void URAnimInstanceBase::NativeInitializeAnimation()
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
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &URAnimInstanceBase::ScopingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetSpecialAttackAimingTag()).AddUObject(this, &URAnimInstanceBase::SpecialAimingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetUltimateAttackAimingTag()).AddUObject(this, &URAnimInstanceBase::UltimateAimingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetAttackingTag()).AddUObject(this, &URAnimInstanceBase::AttackingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetFlyingTag()).AddUObject(this, &URAnimInstanceBase::FlyingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetTiredFlyingTag()).AddUObject(this, &URAnimInstanceBase::TiredFlyingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetHoldingJump()).AddUObject(this, &URAnimInstanceBase::HoldingJumpTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetDeadTag()).AddUObject(this, &URAnimInstanceBase::DeathTagChanged);
		}
	}

	if (StartMontage)
	{
		Montage_Play(StartMontage);
	}
}

void URAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (OwnerCharacter && OwnerMovementComp)
	{
		Speed = OwnerCharacter->GetVelocity().Length();
		DotFlyStamina = Cast<ARPlayerBase>(OwnerCharacter)->DotFlyStamina;

		FVector airVelocity = OwnerCharacter->GetVelocity();
		airVelocity.Z = 0;
		AirSpeed = airVelocity.Length();

		bIsJumping = OwnerMovementComp->IsFalling();

		FRotator characterRot = OwnerCharacter->GetActorRotation();
		FRotator lookRot = OwnerCharacter->GetViewRotation();

		LookOffset = (lookRot - characterRot).GetNormalized();

		FRotator RotDelta = (characterRot - PrevRot).GetNormalized();
		PrevRot = characterRot;

		YawSpeed = FMath::FInterpTo(YawSpeed, RotDelta.Yaw / DeltaSeconds, DeltaSeconds, 10.f);

		FVector Velocity = OwnerCharacter->GetVelocity();

		FVector LookDir = lookRot.Vector();
		LookDir.Z = 0;
		LookDir.Normalize();
		FwdSpeed = Velocity.Dot(LookDir);
		RightSpeed = -Velocity.Dot(LookDir.Cross(FVector::UpVector));
	}
}

void URAnimInstanceBase::ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsScoping = NewStackCount != 0;
}

void URAnimInstanceBase::SpecialAimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bSpecialAiming = NewStackCount != 0;
}

void URAnimInstanceBase::UltimateAimingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bUltimateAiming = NewStackCount != 0;
}

void URAnimInstanceBase::FlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bFlying = NewStackCount != 0;
}

void URAnimInstanceBase::TiredFlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bTiredFlying = NewStackCount != 0;
}

void URAnimInstanceBase::HoldingJumpTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bHoldingJump = NewStackCount != 0;
}

void URAnimInstanceBase::DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bDead = NewStackCount != 0;
}

void URAnimInstanceBase::AttackingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bAttacking = NewStackCount != 0;
}
