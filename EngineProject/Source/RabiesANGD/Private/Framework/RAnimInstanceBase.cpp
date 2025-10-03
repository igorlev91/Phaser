// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RAnimInstanceBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/Character.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"

bool URAnimInstanceBase::ShouldDoUpperBody() const
{
	return (IsMoving() || IsJumping() || GetIsAiming());
}

bool URAnimInstanceBase::ShouldDotBeFlapping() const
{
	return (GetFlying() && GetIsAiming());
}

float URAnimInstanceBase::GetMovementSpeedScale() const
{
	if (FMath::IsNearlyZero(BaseMovementSpeedScale))
	{
		return 1.0f;
	}

	return CurrentMovementSpeedScale / BaseMovementSpeedScale;
}

float URAnimInstanceBase::GetRevivingSpeedScale() const
{
	if (!OwnerCharacter)
	{
		return 1.0f;
	}

	const ARPlayerBase* ownerCharacterBase = Cast<ARPlayerBase>(OwnerCharacter);
	if (!ownerCharacterBase)
	{
		return 1.0f;
	}

	bool bFound = false;

	if (const UAbilitySystemComponent* ASC = ownerCharacterBase->GetAbilitySystemComponent())
	{
		float revivingSpeed = ASC->GetGameplayAttributeValue(URAttributeSet::GetReviveSpeedAttribute(), bFound);
		if (bFound)
		{
			return revivingSpeed;
		}
	}

	return 1.0f;
}

void URAnimInstanceBase::SetOwnerCharacter(USkeletalMeshComponent* MeshComp)
{
	if (OwnerCharacter)
	{
		if (MeshComp)
		{
			if (MeshComp->GetAnimInstance())
			{
				URAnimInstanceBase* RAnim = Cast<URAnimInstanceBase>(MeshComp->GetAnimInstance());
				if (RAnim)
				{
					RAnim->AssignOwner(OwnerCharacter);
				}
			}
		}
	}

}

void URAnimInstanceBase::AssignOwner(const ACharacter* NewOwner)
{
	OwnerCharacter = NewOwner;
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void URAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwnerCharacter == nullptr)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	}
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
		PrevRot = OwnerCharacter->GetActorRotation();
		UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
		if (OwnerASC)
		{
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &URAnimInstanceBase::ScopingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetRevivingTag()).AddUObject(this, &URAnimInstanceBase::RevivingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetSpecialAttackAimingTag()).AddUObject(this, &URAnimInstanceBase::SpecialAimingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetUltimateAttackAimingTag()).AddUObject(this, &URAnimInstanceBase::UltimateAimingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetAttackingTag()).AddUObject(this, &URAnimInstanceBase::AttackingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetFlyingTag()).AddUObject(this, &URAnimInstanceBase::FlyingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetTiredFlyingTag()).AddUObject(this, &URAnimInstanceBase::TiredFlyingTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetHoldingJump()).AddUObject(this, &URAnimInstanceBase::HoldingJumpTagChanged);
			OwnerASC->RegisterGameplayTagEvent(URAbilityGenericTags::GetDeadTag()).AddUObject(this, &URAnimInstanceBase::DeathTagChanged);
			
			BaseFlyingSpeedScale = 830.0f; // very sad this is hard coded but it will never become relavent again... :)
			BaseMovementSpeedScale = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
			//UE_LOG(LogTemp, Error, TEXT("Base Speed scale: %f"), BaseMovementSpeedScale);
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
		const ARPlayerBase* playerBase = Cast<ARPlayerBase>(OwnerCharacter);
		if (playerBase)
		{
			DotFlyStamina = playerBase->DotFlyStamina;
		}

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

		if (bFlying)
			CurrentFlyingSpeedScale = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		CurrentMovementSpeedScale = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
	}
}

void URAnimInstanceBase::ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bIsScoping = NewStackCount != 0;
}

void URAnimInstanceBase::RevivingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bReviving = NewStackCount != 0;
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
