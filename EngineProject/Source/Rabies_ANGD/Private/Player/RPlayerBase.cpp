// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RPlayerBase.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Player/RPlayerController.h"
#include "Framework/EOSPlayerState.h"
#include "Actors/ItemPickup.h"

#include "Widgets/HealthBar.h"

#include "Actors/DamagePopupActor.h"

#include "Player/Chester/ChesterBallActor.h"

#include "Actors/EscapeToWin.h"
#include "Widgets/ReviveUI.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Framework/EOSActionGameState.h"

#include "Framework/RItemDataAsset.h"

#include "Player/Tex/TexKnifesActor.h"

#include "Actors/ItemChest.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

#include "Net/UnrealNetwork.h"

#include "Components/WidgetComponent.h"

#include "Framework/RGameMode.h"

#include "Player/BoltHead/RBoltHead_Actor.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

#include "Framework/RSaveGame.h"


#define ECC_PingInput ECC_GameTraceChannel4

ARPlayerBase::ARPlayerBase()
{

	viewSpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	viewSpringArm->SetupAttachment(GetRootComponent());
	viewSpringArm->bDoCollisionTest = false;
	viewSpringArm->bUsePawnControlRotation = true;
	viewSpringArm->TargetArmLength = 70.0f;

	viewCamera = CreateDefaultSubobject<UCameraComponent>("View Camera");
	viewCamera->SetupAttachment(viewSpringArm, USpringArmComponent::SocketName);

	viewPivot = CreateDefaultSubobject<USceneComponent>("Camera Pivot");
	viewPivot->SetupAttachment(viewSpringArm, USpringArmComponent::SocketName);

	shakePivot = CreateDefaultSubobject<USceneComponent>("Shake Pivot");

	shakePivot->SetupAttachment(viewSpringArm, USpringArmComponent::SocketName);

	viewCamera->SetupAttachment(shakePivot);

	//viewCamera->SetRelativeLocation(DefaultCameraLocal);
	//viewCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(1080.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.0f;
	

	ReviveUIWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Revive Widget Comp");
	ReviveUIWidgetComp->SetupAttachment(GetRootComponent());

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComp->SetupAttachment(GetRootComponent());
	AudioComp->bAutoActivate = false;

	// sphere radius
	ItemPickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Item Collider"));
	ItemPickupCollider->SetupAttachment(GetRootComponent());
	ItemPickupCollider->SetCollisionProfileName(TEXT("OverlapAll"));

	cameraClampMax = 50;
	cameraClampMin = -80;
	bIsScoping = false;

	GroundCheckComp = CreateDefaultSubobject<USphereComponent>("Ground Check Comp");
	GroundCheckComp->SetupAttachment(GetRootComponent());
	GroundCheckComp->OnComponentBeginOverlap.AddDynamic(this, &ARPlayerBase::GroundCheckCompOverlapped);
}

void ARPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//viewPivot->SetRelativeLocation(GetActorLocation()); // centers the pivot on the player without getting the players rotation
	
	if (EOSPlayerState)// && IsLocallyControlled())
	{
		if (dotHijack)
		{
			EOSPlayerState->Server_UpdateDotCenterLocation(dotHijack->GetMesh()->GetSocketLocation("grabPoint"));
		}

		//UE_LOG(LogTemp, Error, TEXT(""), *GetName());
		SetAllyHealthBars();
		//viewPivot->SetRelativeLocation(GetActorLocation()); // centers the pivot on the player without getting the players rotation
		EOSPlayerState->Server_UpdatePlayerVelocity(GetCharacterMovement()->Velocity);
		EOSPlayerState->Server_UpdateSocketLocations(GetMesh()->GetSocketLocation(RootAimingSocketName), GetMesh()->GetSocketLocation(RangedAttackSocketName));
		
		if(bAlternateAim)
			EOSPlayerState->Server_UpdateAlternateAiming(GetMesh()->GetSocketLocation(RangedAltAttackSocketName));

		/*if (bIsScoping)
		{
			RotatePlayer(DeltaTime);
		}*/
	}

}


void ARPlayerBase::SetAllyHealthBars()
{
	FVector viewLoc;
	FRotator viewRot;

	playerController->GetPlayerViewPoint(viewLoc, viewRot);

	// Find all actors in the world
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(playerController->GetWorld(), ARPlayerBase::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		ARPlayerBase* allyPlayer = Cast<ARPlayerBase>(Actor);

		if (!allyPlayer || allyPlayer == this)
			continue;

		allyPlayer->SetHealthBarFromAllyPerspective(GetActorLocation());
	}
}

void ARPlayerBase::FocusGame()
{
	FInputModeGameOnly input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(input);
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
}

void ARPlayerBase::SetDisplayName_Implementation()
{
	if (EOSPlayerState)
	{
		EOSPlayerState->SetPlayerDisplayName(EOSPlayerState->GetPlayerName());
		HealthBar->SetAllyDisplayName(EOSPlayerState->GetPlayerName());
	}
}

void ARPlayerBase::CheckGameOver()
{
	if (EOSPlayerState == nullptr || playerController == nullptr)
	{
		return;
	}

	// Find all actors in the world
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(playerController->GetWorld(), ARPlayerBase::StaticClass(), AllActors);

	bool someonesAlive = false;

	for (AActor* Actor : AllActors)
	{
		ARPlayerBase* allyPlayer = Cast<ARPlayerBase>(Actor);

		if (!allyPlayer)
			continue;

		if (allyPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()) == false)
			someonesAlive = true;
	}

	if (someonesAlive)
		return;

	playerController->GameOver();
	GetWorldTimerManager().ClearTimer(GameOverHandle);

}

void ARPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	Sensitvitiy = 0.5f;
	USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
	if (baseSave)
	{
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame)
		{
			Sensitvitiy = LoadedGame->Sensitivity;
		}
	}

	GetWorldTimerManager().SetTimer(GameOverHandle, this, &ARPlayerBase::CheckGameOver, 2.0f, true);

	FTimerHandle focusOnGame;
	GetWorldTimerManager().SetTimer(focusOnGame, this, &ARPlayerBase::FocusGame, 0.3f, false);

	FTimerHandle Display1;
	GetWorldTimerManager().SetTimer(Display1, this, &ARPlayerBase::SetDisplayName, 1.f, false);

	FTimerHandle Display2;
	GetWorldTimerManager().SetTimer(Display2, this, &ARPlayerBase::SetDisplayName, 5.f, false);

	FTimerHandle DisplayFinal;
	GetWorldTimerManager().SetTimer(DisplayFinal, this, &ARPlayerBase::SetDisplayName, 20.f, false);

	attackSpeedBeforeSpecial = 1.0f;

	PausingInputAction->bTriggerWhenPaused = true;
	FInputModeGameOnly input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(input);

	ItemPickupCollider->OnComponentBeginOverlap.AddDynamic(this, &ARPlayerBase::OnOverlapBegin);
	ItemPickupCollider->OnComponentEndOverlap.AddDynamic(this, &ARPlayerBase::OnOverlapEnd);

	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetLevelAttribute()).AddUObject(this, &ARPlayerBase::LevelUpdated);

	OnDeadStatusChanged.AddUObject(this, &ARPlayerBase::DeadStatusUpdated);
	OnInvisStatusChanged.AddUObject(this, &ARPlayerBase::InvisStatusUpdated);
	OnDealtDamage.AddUObject(this, &ARPlayerBase::DealtDamageCameraShake);
	OnDamageRecieved.AddUObject(this, &ARPlayerBase::RecievedDamage);

	DynamicTexMaterialInstance = UMaterialInstanceDynamic::Create(TexStealthMat, GetMesh());

	if (bMakeTexKnives)
	{
		TexKnives = GetWorld()->SpawnActor<ATexKnifesActor>(TexKnivesClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (TexKnives)
		{
			TexKnives->SetOwner(this);
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			TexKnives->AttachToComponent(GetMesh(), AttachmentRules, "Cup_R");
			TexKnives->SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
			TexKnives->SetActiveState(true);
		}
	}

	if (bMakeChesterBall)
	{
		GetWorldTimerManager().SetTimer(ChesterHealthHandle, this, &ARPlayerBase::CheckChesterHeal, 0.2f, true);

		ChesterBall = GetWorld()->SpawnActor<AChesterBallActor>(ChesterBallClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (ChesterBall)
		{
			ChesterBall->SetOwner(this);
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			ChesterBall->AttachToComponent(GetMesh(), AttachmentRules, "Cup_L");
			ChesterBall->SetActiveState(true);
		}
	}

	ReviveUIWidgetComp->SetWidgetClass(ReviveUIClass);
	ReviveUI = CreateWidget<UReviveUI>(GetWorld(), ReviveUIWidgetComp->GetWidgetClass());
	if (ReviveUI)
	{
		ReviveUIWidgetComp->SetWidget(ReviveUI);
	}
	if (!ReviveUI)
	{
		UE_LOG(LogTemp, Error, TEXT("%s can't spawn revive has the wrong widget setup"), *GetName());
		return;
	}

	ReviveUI->SetRenderScale(FVector2D{ 2.5f });
	ReviveUI->SetVisibility(ESlateVisibility::Hidden);

	if (bInstantPassive)
	{
		GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive);
	}
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
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Triggered, this, &ARPlayerBase::DoAutomaticAttack);
		enhancedInputComp->BindAction(basicAttackAction, ETriggerEvent::Completed, this, &ARPlayerBase::StopBasicAttack);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Started, this, &ARPlayerBase::EnableScoping);
		enhancedInputComp->BindAction(scopeInputAction, ETriggerEvent::Completed, this, &ARPlayerBase::DisableScoping);
		enhancedInputComp->BindAction(scrollInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Scroll);
		enhancedInputComp->BindAction(specialAttackAction, ETriggerEvent::Started, this, &ARPlayerBase::TryActivateSpecialAttack);
		enhancedInputComp->BindAction(specialAttackAction, ETriggerEvent::Completed, this, &ARPlayerBase::FinishSpecialAttack);
		enhancedInputComp->BindAction(ultimateAttackAction, ETriggerEvent::Started, this, &ARPlayerBase::TryActivateUltimateAttack);
		enhancedInputComp->BindAction(ultimateAttackAction, ETriggerEvent::Completed, this, &ARPlayerBase::FinishUltimateAttack);
		enhancedInputComp->BindAction(InteractInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Interact);
		enhancedInputComp->BindAction(PausingInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Pause);
		enhancedInputComp->BindAction(PingInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::Ping);
		enhancedInputComp->BindAction(LoadDebugInputAction, ETriggerEvent::Triggered, this, &ARPlayerBase::LoadDebug);
	}
}

void ARPlayerBase::Move(const FInputActionValue& InputValue)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUnActionableTag()) || GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

	MoveInput = InputValue.Get<FVector2D>();
	MoveInput.Normalize();

	AddMovementInput(MoveInput.Y * GetMoveFwdDir() + MoveInput.X * GetMoveRightDir());
}

void ARPlayerBase::RotatePlayer(float DeltaTime)
{
	//FRotator cameraRot = viewPivot->GetComponentRotation();
	//cameraRot.Roll = 0;

	//FRotator currentRot = GetActorRotation();

	//float yawDiff = cameraRot.Yaw - currentRot.Yaw;
	//float pitchDiff = cameraRot.Pitch - currentRot.Pitch;

	//if (yawDiff > 180.0f) yawDiff -= 360.0f;
	//if (yawDiff < -180.0f) yawDiff += 360.0f;

	//if (pitchDiff > 180.0f) pitchDiff -= 360.0f;
	//if (pitchDiff < -180.0f) pitchDiff += 360.0f;

	//float yawInput = FMath::Lerp(0.0f, yawDiff, 10 * DeltaTime);
	//float pitchInput = FMath::Lerp(0.0f, pitchDiff, 10 * DeltaTime);

	//AddControllerYawInput(yawInput);
	//AddControllerPitchInput(pitchInput);
}

void ARPlayerBase::Look(const FInputActionValue& InputValue)
{
	FVector2D input = InputValue.Get<FVector2D>();
	//input.X *= Sensitvitiy + 0.5f;
	//input.Y *= Sensitvitiy + 0.5f ;

	AddControllerYawInput(input.X * (Sensitvitiy + 0.2f));
	AddControllerPitchInput(-input.Y * (Sensitvitiy + 0.2f));

	/*FRotator newRot = viewPivot->GetComponentRotation();
	newRot.Pitch += input.Y;
	newRot.Yaw += input.X;

	newRot.Pitch = FMath::ClampAngle(newRot.Pitch, cameraClampMin, cameraClampMax);

	if (EOSPlayerState && IsLocallyControlled()) 
	{
		EOSPlayerState->Server_UpdateHitscanRotator(newRot, viewPivot->GetRelativeLocation());
	}
	viewPivot->SetWorldRotation(newRot);*/
}


void ARPlayerBase::SetRabiesPlayerController(ARPlayerController* newController)
{
	playerController = newController;

}

void ARPlayerBase::StartJump()
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUnActionableTag()) || GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;


	if (!IsLocallyControlled())
	{
		return;
	}
	
	if (bInRangeToRevive)
	{
		/*bool canRevive = true;

		TArray<AActor*> playersRevived = nearbyFaintedActors;
		for (AActor* player : playersRevived)
		{
			ARPlayerBase* playerBase = Cast<ARPlayerBase>(player);
			if (playerBase == nullptr)
				continue;

			if (playerBase->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullyDeadTag()))
				canRevive = false;
		}*/

		//if (canRevive)
		//{
			GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Revive);
			return;
		//}
	}

	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::HoldJump);

	if (bInstantJump)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			AudioComp->SetSound(JumpAudio);
			AudioComp->Play();
		}
		else if(bHasDoubleCheck) // is toni
		{
			GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive); // Toni's double jump
		}

		Jump();
		return;
	}

	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Passive); // Dot's jump fly
}

void ARPlayerBase::ReleaseJump()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndTakeOffChargeTag(), eventData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetEndRevivingTag(), eventData);

	if (bInstantJump) return;

	if (!IsFlying())
	{
		if (AudioComp && JumpAudio)
		{
			AudioComp->SetSound(JumpAudio);
			AudioComp->Play();
		}

		Jump();
	}
}

void ARPlayerBase::QuitOut()
{
	/*APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}*/
}

void ARPlayerBase::DoAutomaticAttack()
{
	if (IsScoping())
	{
		//if (AudioComp->!IsPlaying())
		//{

		//}
		GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::BasicAttack);
	}
	else
	{
		GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::MeleeAttack);
	}
}

void ARPlayerBase::Ping()
{
	if (playerController == nullptr)
	{
		return;
	}

	FVector viewLoc;
	FRotator viewRot;

	playerController->GetPlayerViewPoint(viewLoc, viewRot);
	if (bIsScoping == false)
	{
		viewLoc = GetActorLocation();
	}

	FVector startPos = viewLoc + viewRot.Vector();
	FVector endPos = startPos + viewRot.Vector() * 8000.0f;

	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = ECC_PingInput;
	FHitResult newHitResult;
	bool hit = GetWorld()->SweepSingleByChannel(newHitResult, startPos, endPos, FQuat::Identity, collisionChannel, collisionShape);
	if (hit)
	{
		FVector hitPoint = newHitResult.ImpactPoint;
		AActor* hitActor = newHitResult.GetActor();
		Server_HandlePing(hitPoint, hitActor);
		//FColor debugColor = FColor::Blue;
		//DrawDebugCylinder(GetWorld(), startPos, endPos, 1.0f, 32, debugColor, false, 0.2f, 0U, 1.0f);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *newHitResult.GetActor()->GetName()));


	}
}

void ARPlayerBase::Server_HandlePing_Implementation(FVector hitPoint, AActor* hitActor)
{
	AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (gameState)
	{
		gameState->Server_Ping(hitPoint, hitActor);
	}
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
	GetWorldTimerManager().ClearTimer(CameraLerpHandle);

	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::SpecialAttack);
}

void ARPlayerBase::FinishSpecialAttack()
{
	GetAbilitySystemComponent()->TargetConfirm();
}

void ARPlayerBase::TryActivateUltimateAttack()
{
	Multicast_SetHoldingUltimate(true);

	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::UltimateAttack);
}

void ARPlayerBase::FinishUltimateAttack()
{
	GetAbilitySystemComponent()->TargetConfirm();

	Multicast_SetHoldingUltimate(false);

}

void ARPlayerBase::Multicast_SetHoldingUltimate_Implementation(bool state)
{
	if (GetPlayerBaseState())
	{
		GetPlayerBaseState()->Server_UpdateHoldingUltimate(state);

		if (state)
		{
			FGameplayEventData eventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetUltimateHold(), eventData);
		}
		else
		{
			FGameplayEventData eventData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, URAbilityGenericTags::GetUltimateRelease(), eventData);
		}
	}
}


void ARPlayerBase::Interact()
{
	PlayerInteraction.Broadcast();

	bool bLucky = false;
	if (interactionChest != nullptr)
	{
		if (interactionChest->ScrapPrice <= GetCurrentScrap())
		{
			bLucky = CashMyLuck();
		}
	}


	FTimerHandle serverRequestInteraction;
	serverRequestInteraction = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::ServerRequestInteraction, interactionChest, escapeToWin, bLucky));
	//ServerRequestInteraction(interactionChest, escapeToWin, bLucky); // if there's lag this might not work... Reconsider your options carefully
}

void ARPlayerBase::Pause()
{
	isPaused = !isPaused;
	if (IsValid(playerController))
	{
		playerController->TogglePauseMenu(isPaused);
	}
}

void ARPlayerBase::LoadDebug()
{
	return;

	UWorld* World = GetWorld();
	
	if (World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("EndGameTestRoom"));
	}
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
	

	if (IsValid(playerController))
		playerController->ChangeCrosshairState(bNewIsAiming);

	if (bNewIsAiming)
	{
		cameraClampMax = 50;
		cameraClampMin = -80;
		LerpCameraToLocalOffset(AimCameraLocalOffset);
	}
	else
	{
		DisableScoping();
		cameraClampMax = 50;
		cameraClampMin = -80;
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

void ARPlayerBase::SetPausetoFalse()
{
	isPaused = false;
}

bool ARPlayerBase::CashMyLuck()
{
	if (playerController == nullptr || bFeelinLucky == false)
		return false;

	bool feelinRealLucky = playerController->CashMyLuck();
	if (feelinRealLucky)
	{
		PlayVoiceLine(LuckyChesterSounds, 100);
	}

	return feelinRealLucky;
}

void ARPlayerBase::PlayPickupAudio()
{
	if (AudioComp && PickupAudio)
	{
		AudioComp->Play();
	}
}

void ARPlayerBase::GroundCheckCompOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && GetCharacterMovement()->IsFalling())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Grounded"));
		
		if (AudioComp && LandAudio)
		{
			AudioComp->SetSound(LandAudio);
			AudioComp->Play();
		}

	}
}

void ARPlayerBase::SetPlayerReviveState_Implementation(bool state)
{
	if (ReviveUI == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set player revive UI"));
		return;
	}

	if (state)
	{
		if (IsLocallyControlled() == false)
		{
			ReviveUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else 
	{
		ReviveUI->SetVisibility(ESlateVisibility::Collapsed);// do collasped instead of hidden
	}
}

void ARPlayerBase::ServerRequestPickupItem_Implementation(AItemPickup* itemPickup, URItemDataAsset* itemAsset)
{
	if (itemPickup && itemAsset)
	{
		itemPickup->Server_PickupItem();
	}
}

void ARPlayerBase::ServerRequestInteraction_Implementation(AItemChest* Chest, AEscapeToWin* winPoint, bool bLucky)
{
	if (Chest)
	{
		Chest->Server_OpenChest(bLucky);
	}

	if (winPoint)
	{
		if (!winPoint->bStartBoss)
		{
			winPoint->CheckKeyCard();
		}
		else
		{
			winPoint->UseKeycard();
		}
	}
}

void ARPlayerBase::SetPlayerState()
{
	EOSPlayerState = Cast<AEOSPlayerState>(GetPlayerState());
	if (EOSPlayerState)
	{
		EOSPlayerState->Server_OnPossessPlayer(this);
	}
}


AEOSPlayerState* ARPlayerBase::GetPlayerBaseState() // this will crash if someone other than the player calls. Other players do not have this replicated.
{
	return EOSPlayerState;
}

void ARPlayerBase::StartToniRevertSpecialCooldown()
{
	FTimerHandle revertSpecialCooldown;
	GetWorldTimerManager().SetTimer(revertSpecialCooldown, this, &ARPlayerBase::RevertSpecialCooldown, 4.0f, false);

}

void ARPlayerBase::RevertSpecialCooldown()
{
	bool bFound = false;
	float attackCooldown = GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetRangedAttackCooldownReductionAttribute(), bFound);

	if (bFound == false)
	{
		return;
	}

	FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ToniSpecialCooldown, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

	FGameplayEffectSpec* spec = specHandle.Data.Get();
	if (spec)
	{
		float magnitude =  attackSpeedBeforeSpecial - attackCooldown;
		//UE_LOG(LogTemp, Error, TEXT("%f"), attackSpeedBeforeSpecial);
		spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), magnitude);
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
	}
}

void ARPlayerBase::OpenToyBox(AItemPickup* itemToChange, FString itemSelection)
{
	if (playerController && itemToChange)
	{
		playerController->OpenToyBox(itemToChange, itemSelection);
	}
}

void ARPlayerBase::PlayVoiceLine(TArray<USoundBase*> soundEffects, int activationChance)
{

	if (soundEffects.Num() <= 0) return;

	if (activationChance <= FMath::RandRange(0, 99))
		return;


	int32 Index = FMath::RandRange(0, soundEffects.Num() - 1);
	USoundBase* SelectedSound = soundEffects[Index];

	if (HasAuthority())
	{
		AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
		if (GameState)
		{
			FVector Location = GetActorLocation();
			FRotator Rotation = FRotator::ZeroRotator;
			USoundAttenuation* Attenuation = nullptr;

			GameState->Multicast_RequestPlayAudio(SelectedSound, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
		}
	}
	else
	{
		Server_PlayVoiceLine(SelectedSound);
	}
}

void ARPlayerBase::Server_PlayVoiceLine_Implementation(USoundBase* soundEffect)
{
	AEOSActionGameState* GameState = GetWorld()->GetGameState<AEOSActionGameState>();
	if (GameState)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = FRotator::ZeroRotator;
		USoundAttenuation* Attenuation = nullptr;

		GameState->Multicast_RequestPlayAudio(soundEffect, Location, Rotation, 1.0f, 1.0f, 0.0f, Attenuation);
	}
}

void ARPlayerBase::LevelUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (bMakeChesterBall && GetPlayerBaseState())
	{
		float levelUpdated = ChangeData.NewValue - ChangeData.OldValue;
		float healthRegen = ChangeData.NewValue * (levelUpdated * 10.0f);

		FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ChesterPassiveAdjustment, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());
		FGameplayEffectSpec* spec = specHandle.Data.Get();
		if (spec)
		{
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetExpTag(), healthRegen);
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		}
	}
}

void ARPlayerBase::SetInteractionChest_Implementation(AItemChest* chest)
{
	interactionChest = chest;
}

void ARPlayerBase::SetInteractionWin(AEscapeToWin* winPoint)
{
	escapeToWin = winPoint;
}

void ARPlayerBase::SetItemPickup(AItemPickup* itemPickup, URItemDataAsset* itemAsset)
{
	ServerRequestPickupItem(itemPickup, itemAsset);
}

void ARPlayerBase::AddNewItem_Implementation(URItemDataAsset* newItemAsset)
{
	playerController->AddNewItemToUI(newItemAsset);
}

void ARPlayerBase::DeadStatusUpdated(bool bIsDead)
{

	if (bIsDead)
	{
		if (EOSPlayerState)
		{
			PlayVoiceLine(DeathSound, 100);
			StartCameraShake(15.0f, 0.15f);
		}

		Server_HandlePing(FVector(0,0,0), this);
	}
	else
	{

	}
}

void ARPlayerBase::InvisStatusUpdated(bool bIsDead)
{
	if (bIsDead)
	{
		if (DynamicTexMaterialInstance)
		{
			GetMesh()->SetMaterial(0, DynamicTexMaterialInstance);
		}
	}
	else
	{
		GetMesh()->SetMaterial(0, TexDefaultMat);
	}
}

USkeletalMeshComponent* ARPlayerBase::GetItemAttachmentMesh(FName itemName)
{
	TArray<USkeletalMeshComponent*> SkeletalComponents;
	GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	USkeletalMeshComponent* torsoMesh = nullptr;

	for (USkeletalMeshComponent* Skeletal : SkeletalComponents)
	{
		if (Skeletal)
		{
			if (Skeletal->GetName() == TEXT("Torso"))
			{
				torsoMesh = Skeletal;
			}
		}
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* actor : AttachedActors)
	{
		ARBoltHead_Actor* boltHeadActor = Cast<ARBoltHead_Actor>(actor);
		if (boltHeadActor)
		{
			if (itemName == "Friendship Bracelet")
				return boltHeadActor->GetHeadMesh();

			if (itemName == "Hard Hat")
				return boltHeadActor->GetHeadMesh();

			if (itemName == "Radio")
				return boltHeadActor->GetHeadMesh();

			if (itemName == "Spray Bottle")
				return boltHeadActor->GetHeadMesh();
		}
	}

	if (itemName == "Ammo Box")
		return GetMesh();

	if (itemName == "Blood Bag")
		return GetMesh();

	if (itemName == "Canned Food")
		return GetMesh();

	if (itemName == "Rod of Uranium")
		return GetMesh();


	if (torsoMesh)
	{
		return torsoMesh;
	}

	return GetMesh();
}


void ARPlayerBase::CheckChesterHeal()
{

	if (HasAuthority())
	{
		TArray<FOverlapResult> OverlappingResults;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(500);
		FCollisionQueryParams QueryParams;

		AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();

		if (gameState)
		{
			if (gameState->PlayerArray.Num() > 1)
			{
				QueryParams.AddIgnoredActor(this); // Ignore self
			}
		}
		else
		{
			QueryParams.AddIgnoredActor(this); // Ignore self

		}

		//DrawDebugSphere(GetWorld(), GetActorLocation(), 300, 32, FColor::Green, false, 0.1f);

		bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

		TArray<ARPlayerBase*> alreadyHealedPlayers;

		for (const FOverlapResult& result : OverlappingResults)
		{
			ARPlayerBase* player = Cast<ARPlayerBase>(result.GetActor());
			if (player)
			{
				if (alreadyHealedPlayers.Contains(player) == false)
				{
					if (player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullHealthTag()) || player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
						return;

					FGameplayEffectSpecHandle specHandle2 = GetAbilitySystemComponent()->MakeOutgoingSpec(ChesterPassiveAdjustment, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());
					FGameplayEffectSpec* spec2 = specHandle2.Data.Get();
					if (spec2 == nullptr)
						return;

					FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ChesterHealEffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());
					FGameplayEffectSpec* spec = specHandle.Data.Get();
					if (spec == nullptr)
						return;

					bool bFound = false;
					float healing = GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetPassiveHealingAttribute(), bFound);

					if (bFound == false)
						return;

					float level = GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetLevelAttribute(), bFound);

					if (bFound == false)
						return;

					float healthRegen = FMathf::Clamp(healing, 0, level);

					if (gameState && ChesterHealParticle && healthRegen > 0)
					{
						FVector SpawnPos = GetActorLocation();
						SpawnPos.Z -= 23;
						gameState->Multicast_ShootTexUltimate(ChesterHealParticle, this, SpawnPos, player->GetActorLocation(), player->GetActorLocation());
					}

					spec2->SetSetByCallerMagnitude(URAbilityGenericTags::GetExpTag(), -healthRegen);
					GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec2);

					AddToHealingDone(player, healthRegen);

					spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetExpTag(), healthRegen);
					player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);

					alreadyHealedPlayers.Add(player);
					
				}
			}
		}
	}
}

void ARPlayerBase::FrameDelayItemPickup(AItemPickup* newItem)
{
	if (newItem->ItemAsset)
	{
		if (newItem->ItemAsset->ItemTooltip == "CommonToyBox" || newItem->ItemAsset->ItemTooltip == "UncommonToyBox" || newItem->ItemAsset->ItemTooltip == "RareToyBox")
		{
			OpenToyBox(newItem, newItem->ItemAsset->ItemTooltip);
			return;
		}
	}

	newItem->PlayerPickupRequest(this);
}


void ARPlayerBase::NetMulticast_SetDotHijack_Implementation(ARPlayerBase* dot)
{
	dotHijack = dot;
}

void ARPlayerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetUnActionableTag()) || GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) return;

	AItemPickup* newItem = Cast<AItemPickup>(OtherActor);
	if (newItem)
	{
		PickupItemHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::FrameDelayItemPickup, newItem));
	}

	ARPlayerBase* player = Cast<ARPlayerBase>(OtherActor);
	if (player && player != this)
	{
		if (player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetFullyDeadTag()))
		{
			if (nearbyFaintedActors.Contains(OtherActor))
				nearbyFaintedActors.Remove(OtherActor);

			return;
		}

		if (!nearbyFaintedActors.Contains(OtherActor))
			nearbyFaintedActors.Add(OtherActor);

		if (player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		{
			bInRangeToRevive = true;
			player->SetPlayerReviveState(true);
		}
	}
}

void ARPlayerBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPlayerBase* player = Cast<ARPlayerBase>(OtherActor);
	if (player && player != this)
	{
		if (nearbyFaintedActors.Contains(OtherActor))
			nearbyFaintedActors.Remove(OtherActor);

		if (player->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
		{
			bInRangeToRevive = false;
			player->SetPlayerReviveState(false);
		}
	}
}

void ARPlayerBase::StartCameraShake(float intensity, float duration)
{
	if (EOSPlayerState == nullptr)
		return;

	GetWorldTimerManager().ClearTimer(ScreenShakeLerpHandle);
	ScreenShakeLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::CameraShake, intensity, duration));
}

void ARPlayerBase::CameraShake(float intensity, float duration)
{
	if (duration <= 0)
	{
		shakePivot->SetRelativeLocation(FVector(0, 0, 0));
		Server_SetCameraShake(shakePivot->GetRelativeLocation());
		return;
	}

	FVector ShakeOffset = FVector(FMath::RandRange(-intensity, intensity),FMath::RandRange(-intensity, intensity),FMath::RandRange(-intensity, intensity));
	FVector ShakePivotLocation = shakePivot->GetRelativeLocation() + ShakeOffset;
	shakePivot->SetRelativeLocation(ShakePivotLocation);
	Server_SetCameraShake(shakePivot->GetRelativeLocation());
	duration -= GetWorld()->GetDeltaSeconds();
	ScreenShakeLerpHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARPlayerBase::CameraShake, intensity, duration));
}

void ARPlayerBase::Server_SetCameraShake_Implementation(FVector pos)
{
	EOSPlayerState->Server_UpdateCameraShake(pos);
}

bool ARPlayerBase::Server_SetCameraShake_Validate(FVector pos)
{
	return true;
}

void ARPlayerBase::DealtDamageCameraShake(ARCharacterBase* damagedCharacter)
{
	StartCameraShake(2.0f, 0.1f);
}

void ARPlayerBase::RecievedDamage(float damage)
{
	if (EOSPlayerState && IsLocallyControlled())
	{
		if (damage >= 2)
		{
			if (DamagePopupActorClass != nullptr)
			{

				FActorSpawnParameters SpawnParams;
				FVector spawnPos = GetActorLocation();
				ADamagePopupActor* newActor = GetWorld()->SpawnActor<ADamagePopupActor>(DamagePopupActorClass, spawnPos, FRotator::ZeroRotator, SpawnParams);
				//newActor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
				int x = FMath::RandRange(-200, 200);
				int z = FMath::RandRange(20, 100);
				FVector randomAnimOffset = GetActorLocation() + FVector(x, 0.0f, z);

				newActor->SetText(damage, randomAnimOffset);
			}
			return;
		}

		if (damage == 0 || damage == 1)
		{
			return;
		}

		PlayVoiceLine(HurtSounds, 30);

		float intensity = FMath::Clamp(damage / 2.0f, 1.0f, 10.0f);
		StartCameraShake(intensity, 0.1f);
	}

}