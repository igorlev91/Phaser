// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerBase.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Player/RPlayerController.h"

#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

#include "Framework/RGameMode.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"

ARPlayerBase::ARPlayerBase()
{
	viewPivot = CreateDefaultSubobject<USceneComponent>("Camera Pivot");
	viewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");

	viewCamera->SetupAttachment(viewPivot, USpringArmComponent::SocketName);
	viewCamera->bUsePawnControlRotation = false;
	viewCamera->SetRelativeLocation(DefaultCameraLocal);

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1080.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.7f;

	cameraClampMax = 10;
	cameraClampMin = -60;
	bIsScoping = false;
}

void ARPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewPivot->SetRelativeLocation(GetActorLocation()); // centers the pivot on the player without getting the players rotation

	if (IsFlying() && GetCharacterMovement()->IsFalling() == false && bHoldingJump == false)
	{
		FGameplayEventData eventData;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndFlyingTag(), eventData);
	}

	if (IsFlying() && bHoldingJump)
	{
		FVector NewVelocity = GetVelocity();
		NewVelocity.Z = FMath::Lerp(NewVelocity.Z, -0.1f, 4 * DeltaTime);
		GetCharacterMovement()->Velocity = NewVelocity;
	}
	
	if (bIsScoping)
	{
		FRotator playerRot = viewPivot->GetRelativeRotation();
		playerRot.Roll = 0;
		SetActorRotation(playerRot); // replciate this movement as well as Movement Component
	}
}

void ARPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	playerController = Cast<ARPlayerController>(GetController());
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
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Started, this, &ARPlayerBase::StartJump);
		enhancedInputComp->BindAction(jumpInputAction, ETriggerEvent::Completed, this, &ARPlayerBase::ReleaseJump);
		enhancedInputComp->BindAction(QuitOutAction, ETriggerEvent::Triggered, this, &ARPlayerBase::QuitOut);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::DoBasicAttack);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Completed, this, &ARPlayerBase::StopBasicAttack);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Started, this, &ARPlayerBase::EnableScoping);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Completed, this, &ARPlayerBase::DisableScoping);
		enhancedInputComp->BindAction(scrollInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Scroll);
		enhancedInputComp->BindAction(specialAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateSpecialAttack);
		enhancedInputComp->BindAction(ultimateAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::TryActivateUltimateAttack);
		enhancedInputComp->BindAction(AbilityConfirmAction, ETriggerEvent::Triggered, this, &ARPlayerBase::ConfirmActionTriggered);
		enhancedInputComp->BindAction(AbilityCancelAction, ETriggerEvent::Triggered, this, &ARPlayerBase::CancelActionTriggered);
		enhancedInputComp->BindAction(InteractInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Interact);
		enhancedInputComp->BindAction(PausingInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Pause);
	}
}

void ARPlayerBase::Move(const FInputActionValue& InputValue)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUnActionableTag()) || GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

	FVector2D input = InputValue.Get<FVector2D>();
	input.Normalize();

	AddMovementInput(input.Y * GetMoveFwdDir() + input.X * GetMoveRightDir());
}

void ARPlayerBase::Look(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();

	FRotator newRot = viewPivot->GetComponentRotation();
	newRot.Pitch += input.Y;
	newRot.Yaw += input.X;

	newRot.Pitch = FMath::ClampAngle(newRot.Pitch, cameraClampMin, cameraClampMax);

	viewPivot->SetWorldRotation(newRot);
}

void ARPlayerBase::StartJump()
{
	if (bInstantJump)
	{
		Jump();
		return;
	}

	bHoldingJump = true;
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive); // Dot's jump fly
}

void ARPlayerBase::ReleaseJump()
{
	if (bInstantJump) return;

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndTakeOffChargeTag(), eventData);

	bHoldingJump = false;
	if (!IsFlying())
	{
		Jump();
	}
}

void ARPlayerBase::QuitOut()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}
}

void ARPlayerBase::DoBasicAttack()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::BasicAttack);
}

void ARPlayerBase::StopBasicAttack()
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndAttackTag(), eventData);
}

void ARPlayerBase::EnableScoping()
{
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Scoping);
}

void ARPlayerBase::DisableScoping()
{
	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndScopingTag(), eventData);
}

void ARPlayerBase::Scroll(const FInputActionValue& InputActionVal)
{

	float scrollAmount = InputActionVal.Get<float>();

	scrollAmount *= -1;

	DefaultCameraLocal.X += (scrollAmount * 100);

	DefaultCameraLocal.X = FMath::Clamp(DefaultCameraLocal.X, -800, -200);

	if (!bIsScoping)
	{
		GetWorldTimerManager().ClearTimer(CameraLerpHandle);
		viewCamera->SetRelativeLocation(DefaultCameraLocal);
	}
}

void ARPlayerBase::TryActivateSpecialAttack()
{

}

void ARPlayerBase::TryActivateUltimateAttack()
{

}

void ARPlayerBase::ConfirmActionTriggered()
{
	//UE_LOG(LogTemp, Warning, TEXT("Confirmed"));
	//GetAbilitySystemComponent()->InputConfirm();
}

void ARPlayerBase::CancelActionTriggered()
{
	//UE_LOG(LogTemp, Warning, TEXT("Cancelled"));
	//GetAbilitySystemComponent()->InputCancel();
}

void ARPlayerBase::Interact()
{
	if (!canInteract)
	{
		UE_LOG(LogTemp, Warning, TEXT("I cannot interact"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("I am interacting"));
}

void ARPlayerBase::Pause()
{
	/*ARGameMode* GameMode = GetWorld()->GetAuthGameMode<ARGameMode>();
	isPaused = !isPaused;

	if (isPaused)
	{
		GameMode->PausingGame(isPaused);
		//Remove all controls to the characters when they are paused.
	}
	else
	{
		GameMode->PausingGame(isPaused);
		//Return all controls to the characters when they are unpaused.
	}*/
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

void ARPlayerBase::ScopingTagChanged(bool bNewIsAiming)
{
	bUseControllerRotationYaw = bNewIsAiming;
	bIsScoping = bNewIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bNewIsAiming;

	if (playerController)
		playerController->ChangeCrosshairState(bNewIsAiming);

	if (bNewIsAiming)
	{
		cameraClampMax = 50;
		cameraClampMin = -50;
		LerpCameraToLocalOffset(AimCameraLocalOffset);
	}
	else
	{
		cameraClampMax = 10;
		cameraClampMin = -60;
		LerpCameraToLocalOffset(DefaultCameraLocal);
	}
}

void ARPlayerBase::LerpCameraToLocalOffset(const FVector& LocalOffset)
{
	GetWorldTimerManager().ClearTimer(CameraLerpHandle);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::TickCameraLocalOffset, LocalOffset));
}

void ARPlayerBase::TickCameraLocalOffset(FVector Goal)
{
	FVector CurrentLocalOffset = viewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1)
	{
		viewCamera->SetRelativeLocation(Goal);
		return;
	}

	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, GetWorld()->GetDeltaSeconds() * AimCameraLerpingSpeed);
	viewCamera->SetRelativeLocation(NewLocalOffset);
	CameraLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::TickCameraLocalOffset, Goal));
}

void ARPlayerBase::SetInteraction(bool setInteract)
{
	canInteract = setInteract;
}

void ARPlayerBase::SetPausetoFalse()
{
	isPaused = false;
}
