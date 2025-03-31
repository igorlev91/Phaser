// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RCharacterBase.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/RGameMode.h"

#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Framework/EOSActionGameState.h"

#include "Player/RPlayerBase.h"
#include "Enemy/REnemyBase.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "Framework/RPushBoxComponent.h"

#include "Framework/EOSPlayerState.h"

#include "Math/Color.h"
#include "Math/UnrealMathUtility.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Widgets/HealthBar.h"

#include "Framework/RAttackingBoxComponent.h"

#include "Net/UnrealNetwork.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Touch.h"

#define ECC_EnemyRangedAttack ECC_GameTraceChannel2
#define ECC_AllyRangedAttack ECC_GameTraceChannel3

// Sets default values
ARCharacterBase::ARCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<URAbilitySystemComponent>("Ability System Component");
	AbilitySystemComponent->SetIsReplicated(true); // replicate means it is synced with the server.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<URAttributeSet>("Attribute Set");

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetLevelAttribute()).AddUObject(this, &ARCharacterBase::LevelUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetExpAttribute()).AddUObject(this, &ARCharacterBase::ExpUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetNextLevelExpAttribute()).AddUObject(this, &ARCharacterBase::NextLevelExpUpdated);


	//
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetHealthAttribute()).AddUObject(this, &ARCharacterBase::HealthUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ARCharacterBase::MaxHealthUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &ARCharacterBase::MovementSpeedUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetGravityAttribute()).AddUObject(this, &ARCharacterBase::GravityUpdated);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAttributeSet::GetForwardSpeedAttribute()).AddUObject(this, &ARCharacterBase::ForwardSpeedUpdated);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetScopingTag()).AddUObject(this, &ARCharacterBase::ScopingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetDeadTag()).AddUObject(this, &ARCharacterBase::DeathTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetFlyingTag()).AddUObject(this, &ARCharacterBase::FlyingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetTakeOffDelayTag()).AddUObject(this, &ARCharacterBase::TakeOffDelayTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetHoldingJump()).AddUObject(this, &ARCharacterBase::HoldingJumpTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(URAbilityGenericTags::GetMeleeAttackingTag()).AddUObject(this, &ARCharacterBase::MeleeAttackingTagChanged);

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Status Widget Comp");
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());

	AttackingBoxComponent = CreateDefaultSubobject<URAttackingBoxComponent>("Attacking Box Component");
	AttackingBoxComponent->SetupAttachment(GetMesh());

	PushingBoxComponent = CreateDefaultSubobject<URPushBoxComponent>("Pushing Box Component");
	PushingBoxComponent->SetupAttachment(GetMesh());

	PrimaryActorTick.bRunOnAnyThread = false; // prevents crash??
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
		GetWorldTimerManager().SetTimer(FriendshipBraceletTimer,this,&ARCharacterBase::CheckFriendShipBracelet, 0.5f,  true );
		// THIS IS FOR HEALING BRACLET, AND WILL WORK FOREVER IF YOU ARE FAINTED

		APlayerController* OwningPlayerController = Cast<APlayerController>(Controller);
		// Find the ID

		TeamId = FGenericTeamId(1);
	}
}

void ARCharacterBase::InitStatusHUD()
{
	HealthBarWidgetComp->SetWidgetClass(HealthBarClass);
	HealthBar = CreateWidget<UHealthBar>(GetWorld(), HealthBarWidgetComp->GetWidgetClass());
	if (HealthBar)
	{
		HealthBarWidgetComp->SetWidget(HealthBar);
	}
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

	if (bBossHealthBar)
	{
		HealthBar->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsLocallyControlled())
	{
		if (GetController() && GetController()->IsPlayerController())
			HealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

int ARCharacterBase::GetCurrentScrap()
{
	return AttributeSet->GetScrap();
}

int ARCharacterBase::GetReviveSpeed()
{
	return AttributeSet->GetReviveSpeed();
}

bool ARCharacterBase::GetKeyCard()
{
	if (AttributeSet->GetKeyCard() >= 1)
		return true;

	return false;
}

void ARCharacterBase::Hitscan(float range, AEOSPlayerState* requestedPlayerState)
{
	FVector startPos = FVector(0, 0, 0);
	FVector endPos = FVector(0, 0, 0);

	bool isEnemy = (requestedPlayerState == nullptr);

	if (isEnemy) // it's an enemy request
	{
		FVector gunSocket = GetMesh()->GetSocketLocation(RangedAttackSocketName);
		FRotator gunRotation = GetMesh()->GetSocketRotation(RangedAttackSocketName);
		startPos = gunSocket + gunRotation.Vector();
		endPos = startPos + gunRotation.Vector() * range;
	}
	else
	{
		startPos = requestedPlayerState->GetRootAimingLocation() + requestedPlayerState->GetHitscanRotator().Vector();
		endPos = startPos + requestedPlayerState->GetHitscanRotator().Vector() * range;
	}

	FCollisionShape collisionShape = FCollisionShape::MakeSphere(1);
	ECollisionChannel collisionChannel = (isEnemy) ? ECC_EnemyRangedAttack : ECC_AllyRangedAttack ;
	bool hit = GetWorld()->SweepSingleByChannel(hitResult, startPos, endPos, FQuat::Identity, collisionChannel, collisionShape);
	if (hit)
	{
		FVector weaponStart = (isEnemy) ? startPos : requestedPlayerState->GetRangedLocation();
		FVector hitEnd = hitResult.ImpactPoint;
		CharacterShootParticle(weaponStart, hitEnd, (hitEnd - weaponStart).GetSafeNormal().Rotation() + FRotator(0, -90, 0)); // this gets the point towards the hit
		ClientHitScanResult(hitResult.GetActor(), weaponStart, hitEnd, isEnemy);
	}
}

void ARCharacterBase::ClientHitScanResult_Implementation(AActor* hitActor, FVector start, FVector end, bool enemy)
{
	FString actorName = hitActor->GetName();
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *actorName));
	//DrawDebugLine(GetWorld(), start, end, FColor::Green);
	FColor debugColor = (enemy) ? FColor::Red : FColor::Green ;
	DrawDebugCylinder(GetWorld(), start, end, 1.0f, 32, debugColor, false, 0.2f, 0U, 1.0f);
	ClientHitScan.Broadcast(hitActor, start, end);
}


bool ARCharacterBase::ClientHitScanResult_Validate(AActor* hitActor, FVector start, FVector end, bool enemy)
{
	return true;
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
	//AbilitySystemComponent->ApplyGameplayEffect(DeathEffect);
	GetAbilitySystemComponent()->PressInputID((int)EAbilityInputID::Dead); // this does not apply the dead tag sadly to enemies I believe
	//AbilitySystemComponent->AddLooseGameplayTag(URAbilityGenericTags::GetDeadTag());
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnDeadStatusChanged.Broadcast(true);
}

void ARCharacterBase::DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	if (NewStackCount == 0) // for getting revived
	{
		PlayAnimMontage(ReviveMontage);
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

void ARCharacterBase::MeleeAttackingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount)
{
	bMeleeAttacking = NewStackCount != 0;
	MeleeAttackingTagChanged(bMeleeAttacking);
}

void ARCharacterBase::LevelUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	if (HealthBar)
	{
		HealthBar->SetLevel(ChangeData.NewValue);
	}

	onLevelUp.Broadcast(ChangeData.NewValue);
}

void ARCharacterBase::ExpUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (HasAuthority())
	{
		if (ChangeData.NewValue >= AttributeSet->GetNextLevelExp())
		{
			LevelUp(ChangeData.NewValue - AttributeSet->GetNextLevelExp());
		}
	}
}

void ARCharacterBase::NextLevelExpUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (HasAuthority())
	{
		if (ChangeData.NewValue <= AttributeSet->GetExp())
		{
			LevelUp(AttributeSet->GetExp() - ChangeData.NewValue);
		}
	}
}

void ARCharacterBase::DealtDamage(ARCharacterBase* hitCharacter)
{

}

void ARCharacterBase::HitSpecialAttack(ARCharacterBase* hitCharacter)
{
	if (HasAuthority() == false)
		return;

	CheckIVBag();
	DealtDamage(hitCharacter);
}

void ARCharacterBase::HitRangedAttack(ARCharacterBase* hitCharacter)
{
	if (HasAuthority() == false)
		return;

	CheckRadio(hitCharacter);
	DealtDamage(hitCharacter);
}

void ARCharacterBase::HitMeleeAttack(ARCharacterBase* hitCharacter)
{
	if (HasAuthority() == false)
		return;

	CheckHardhat();
	CheckNails(hitCharacter);
	DealtDamage(hitCharacter);
}

void ARCharacterBase::CheckFriendShipBracelet_Implementation()
{
	if (HasAuthority())
	{
		HealingRadiusEffect(FriendShipEffect, false);
	}
}

void ARCharacterBase::CheckIVBag()
{
	HealingRadiusEffect(IVBagEffect, true);
}

void ARCharacterBase::CheckHardhat()
{
	bool bFoundLifeSteal = false;
	bool bFoundMeleeStrength = false;
	float lifesteal = AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetBasicAttackLifestealAttribute(), bFoundLifeSteal);
	float meleeStrength = AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetMeleeAttackStrengthAttribute(), bFoundMeleeStrength);

	if (bFoundLifeSteal == false || bFoundMeleeStrength == 0 || lifesteal <= 0)
		return;

	FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(LifestealEffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

	FGameplayEffectSpec* spec = specHandle.Data.Get();
	if (spec)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Healed from melee attack!"), *GetName());
		spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetGenericTargetAquiredTag(), meleeStrength * lifesteal);
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
	}
}

void ARCharacterBase::CheckNails(ARCharacterBase* hitCharacter)
{
	bool bFound = false;
	float nailsChance = AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetNailsEffectChanceAttribute(), bFound);

	if (bFound == false || nailsChance <= 0)
		return;

	float randomApplyChance = FMath::RandRange(0, 100);
	//UE_LOG(LogTemp, Error, TEXT("%f% Trying to inflict got %f"), nailsChance, randomApplyChance);
	if (nailsChance >= randomApplyChance)
	{
		FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(NailsEfffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

		FGameplayEffectSpec* spec = specHandle.Data.Get();
		if (spec)
		{
			hitCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		}
	}
}

void ARCharacterBase::CheckRadio(ARCharacterBase* hitCharacter)
{
	
	bool bFoundHealth = false;
	float health = hitCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetHealthAttribute(), bFoundHealth);

	if (bFoundHealth == false)
		return;

	if (health > 0)
		return;

	bool bFoundRadioEffect = false; // this is used for radius instead of effect
	float radioEffect = AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetRadioEffectChanceAttribute(), bFoundRadioEffect);

	if (bFoundRadioEffect == false)
		return;


	if (radioEffect <= 0)
		return;

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(radioEffect);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(hitCharacter);

	DrawDebugSphere(GetWorld(), hitCharacter->GetActorLocation(), radioEffect, 32, FColor::Red, false, 2.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, hitCharacter->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	TArray<AREnemyBase*> alreadyDamaged;

	for (const FOverlapResult& result : OverlappingResults)
	{
		AREnemyBase* enemy = Cast<AREnemyBase>(result.GetActor());
		if (enemy)
		{
			bool bNewFoundHealth = false;
			float newEnemyhealth = enemy->GetAbilitySystemComponent()->GetGameplayAttributeValue(URAttributeSet::GetHealthAttribute(), bNewFoundHealth);

			if (alreadyDamaged.Contains(enemy) == false && newEnemyhealth > 0)
			{
				FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(RadioEfffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

				FGameplayEffectSpec* spec = specHandle.Data.Get();
				if (spec)
				{
					alreadyDamaged.Add(enemy);
					enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
					RadioDelayTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ARCharacterBase::CheckRadioDelay, enemy));
					//GetWorldTimerManager().SetTimer(RadioDelayTimer, this, &ARCharacterBase::CheckRadioDelay, 0.2f, enemy);
				}
			}
		}
	}
}

void ARCharacterBase::CheckRadioDelay(AREnemyBase* hitCharacter)
{
	CheckRadio(hitCharacter);
}

void ARCharacterBase::HealingRadiusEffect(TSubclassOf<UGameplayEffect> healingEffect, bool IVBag)
{
	bool bFound = false;
	float healingRadius = (IVBag) ? AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetAbilityHealingRadiusAttribute(), bFound) : AbilitySystemComponent->GetGameplayAttributeValue(URAttributeSet::GetFriendshipHealingRadiusAttribute(), bFound);

	if (bFound == false || healingRadius == 0)
		return;

	TArray<FOverlapResult> OverlappingResults;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(healingRadius);
	FCollisionQueryParams QueryParams;
	if (IVBag == false)
	{
		QueryParams.AddIgnoredActor(this); // Ignore self
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), healingRadius, 32, FColor::Green, false, 2.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(OverlappingResults, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, QueryParams);

	TArray<ARPlayerBase*> alreadyHealedPlayers;
	bool foundFriend = false;

	for (const FOverlapResult& result : OverlappingResults)
	{
		ARPlayerBase* player = Cast<ARPlayerBase>(result.GetActor());
		if (player)
		{
			if (alreadyHealedPlayers.Contains(player) == false)
			{
				foundFriend = true;

				FGameplayEffectSpecHandle specHandle = player->GetAbilitySystemComponent()->MakeOutgoingSpec(healingEffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

				FGameplayEffectSpec* spec = specHandle.Data.Get();
				if (spec)
				{
					alreadyHealedPlayers.Add(player);
					player->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
				}
			}
		}
	}

	if (IVBag == false && foundFriend)
	{
		FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(healingEffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

		FGameplayEffectSpec* spec = specHandle.Data.Get();
		if (spec)
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		}
	}
}

void ARCharacterBase::LevelUp(int carryOverEXP) // Handles the player level up
{
	if (LevelUpEffect && GetAbilitySystemComponent())
	{

		FGameplayEffectSpecHandle specHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(LevelUpEffect, 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

		FGameplayEffectSpec* spec = specHandle.Data.Get();
		if (spec)
		{
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetExpTag(), carryOverEXP);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetPrevLevelTag(), AttributeSet->GetNextLevelExp());
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetNextLevelTag(), FMath::CeilToInt(AttributeSet->GetNextLevelExp() * 1.2f)); // This is exponential level curve. 1.2x required every level
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
		}

		LevelUpUpgrade(AttributeSet->GetLevel(), false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO LEVEL UP EFFECT"), *GetName());
	}
}

void ARCharacterBase::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	if (ChangeData.OldValue >= 1)
	{
		bHasDied = false;
	}

	if (ChangeData.NewValue > 0 && ChangeData.NewValue < ChangeData.OldValue && FlinchMontage != nullptr && AirFlinchMontage != nullptr)
	{
		if (bIsFlying)
		{
			ServerPlayAnimMontage(AirFlinchMontage);
		}
		else
		{
			ServerPlayAnimMontage(FlinchMontage);
		}
	}

	if (HealthBar)
	{
		HealthBar->SetHealth(ChangeData.NewValue, AttributeSet->GetMaxHealth());
	}

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

	if (ChangeData.NewValue <= 0 && !bHasDied)
	{
		bHasDied = true;
		StartDeath();
		AEOSActionGameState* gameState = Cast<AEOSActionGameState>(GetWorld()->GetGameState());
		if (HasAuthority() && ChangeData.GEModData && GetOwner() == gameState)
		{
			gameState->AwardEnemyKill(DeathEffect);
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

void ARCharacterBase::ForwardSpeedUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("%s NO ATTRIBUTE SET"), *GetName());
		return;
	}

	if (ChangeData.NewValue == 0)
	{
		FVector currentVelocity = GetCharacterMovement()->Velocity;
		FVector force = (-currentVelocity) * 0.9f;

		GetCharacterMovement()->AddImpulse(force, true);
	}
	else
	{
		FRotator viewRot = GetActorRotation();

		FVector forwardDirection = viewRot.Vector();
		FVector force = forwardDirection * ChangeData.NewValue;

		GetCharacterMovement()->AddImpulse(force, true);
	}
}

void ARCharacterBase::LaunchBozo_Implementation(FVector launchVelocity)
{
	if (HasAuthority())
	{
		GetCharacterMovement()->StopMovementImmediately();
		
		FVector NewLocation = GetActorLocation() + FVector(0, 0, 100);
		SetActorLocation(NewLocation, true);

		LaunchCharacter(launchVelocity, true, true);
		//GetCharacterMovement()->Launch(launchVelocity);
	}
}

void ARCharacterBase::LevelUpUpgrade(int level, bool setLevel)
{
	FGameplayEffectContextHandle contextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle effectSpechandle = GetAbilitySystemComponent()->MakeOutgoingSpec(LevelupUpgradeEffect, 1.0f, contextHandle);

	FGameplayEffectSpec* spec = effectSpechandle.Data.Get();
	if (spec)
	{
		if (setLevel)
		{
			float levelScale = (float)level - 1;
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetLevelTag(), levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetMaxHealthTag(), HealthOnLevelUp * levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetHealthTag(), HealthOnLevelUp * levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetMeleeAttackStrengthTag(), MeleeStrengthOnLevelUp * levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetRangedAttackStrengthTag(), RangedStrengthOnLevelUp * levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetSpecialStrengthTag(), SpeicalStrengthOnLevelUp * levelScale);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetUltimateStrengthTag(), UltimateStrengthOnLevelUp * levelScale);
		}
		else
		{
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetLevelTag(), 0.0f);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetMaxHealthTag(), HealthOnLevelUp);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetHealthTag(), HealthOnLevelUp);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetMeleeAttackStrengthTag(), MeleeStrengthOnLevelUp);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetRangedAttackStrengthTag(), RangedStrengthOnLevelUp);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetSpecialStrengthTag(), SpeicalStrengthOnLevelUp);
			spec->SetSetByCallerMagnitude(URAbilityGenericTags::GetUltimateStrengthTag(), UltimateStrengthOnLevelUp);
		}

		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*spec);
	}

}

void ARCharacterBase::CharacterShootParticle_Implementation(FVector startPos, FVector endPos, FRotator startForward)
{
	///
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

AActor* ARCharacterBase::GetTarget()
{
	return AITarget;
}

void ARCharacterBase::UpdateAITarget_Implementation(AActor* newTargetActor)
{
	AITarget = newTargetActor;
}

void ARCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARCharacterBase, TeamId, COND_None);
	DOREPLIFETIME_CONDITION(ARCharacterBase, AITarget, COND_None);
	DOREPLIFETIME_CONDITION(ARCharacterBase, AILevel, COND_None);
}