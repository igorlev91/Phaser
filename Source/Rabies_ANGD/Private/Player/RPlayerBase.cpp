// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerBase.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Net/UnrealNetwork.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ARPlayerBase::ARPlayerBase()
{
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	viewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");

	cameraBoom->SetupAttachment(GetRootComponent());
	viewCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

	cameraBoom->bUsePawnControlRotation = true;
	cameraBoom->TargetArmLength = 800.f;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1080.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void ARPlayerBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(inputMapping, 0);
	}
}

void ARPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* enhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComp)
	{
		enhancedInputComp->BindAction(moveInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Move);
		enhancedInputComp->BindAction(lookInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Look);
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Jump);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::DoBasicAttack);
		enhancedInputComp->BindAction(specialAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateSpecialAttack);
		enhancedInputComp->BindAction(ultimateAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateUltimateAttack);
		enhancedInputComp->BindAction(AbilityConfirmAction, ETriggerEvent::Triggered, this, &ARPlayerBase::ConfirmActionTriggered);
		enhancedInputComp->BindAction(AbilityCancelAction, ETriggerEvent::Triggered, this, &ARPlayerBase::CancelActionTriggered);
	}
}

void ARPlayerBase::Move(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();
	input.Normalize();

	AddMovementInput(input.Y * GetMoveFwdDir() + input.X * GetMoveRightDir());
}

void ARPlayerBase::Look(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();
	AddControllerYawInput(input.X);
	AddControllerPitchInput(-input.Y);
}

void ARPlayerBase::DoBasicAttack()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::BasicAttack);
}

void ARPlayerBase::TryActivateSpecialAttack()
{

}

void ARPlayerBase::TryActivateUltimateAttack()
{

}

void ARPlayerBase::ConfirmActionTriggered()
{

}

void ARPlayerBase::CancelActionTriggered()
{

}

FVector ARPlayerBase::GetMoveFwdDir() const
{
	FVector CamerFwd = viewCamera->GetForwardVector();
	CamerFwd.Z = 0;
	return CamerFwd.GetSafeNormal();
}

FVector ARPlayerBase::GetMoveRightDir() const
{
	return viewCamera->GetRightVector();
}
