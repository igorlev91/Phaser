// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilities/RGameplayCueInterface.h"

#include "GenericTeamAgentInterface.h"

#include "RCharacterBase.generated.h"

class URAbilitySystemComponent;
class URAttributeSet;
class UGameplayEffect;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeadStatusChanged, bool /*bIsDead*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int /*new level*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnClientHitScan, AActor* /*Hit Target*/, FVector /* Start Pos */, FVector /* End Pos */);

UCLASS()
class ARCharacterBase : public ACharacter, public IAbilitySystemInterface, /*public IRGameplayCueInterface,*/ public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FOnDeadStatusChanged OnDeadStatusChanged;

	FOnClientHitScan ClientHitScan;
	FOnLevelUp onLevelUp;

	// Sets default values for this character's properties
	ARCharacterBase();

	UFUNCTION(BlueprintCallable)
	void SetupAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
	void InitAttributes();

	UFUNCTION(BlueprintCallable)
	void InitAbilities();

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	UFUNCTION()
	void Hitscan(float range, class AEOSPlayerState* requestedPlayerState);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CharacterShootParticle(FVector startPos, FVector endPos, FRotator startForward);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bBossHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName bBossName;

protected:

	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	void InitStatusHUD();

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	int GetCurrentScrap();

	UFUNCTION()
	int GetReviveSpeed();

	UFUNCTION()
	bool GetKeyCard();

	FName RangedAttackSocketName = TEXT("Ranged_Socket");
	FName RootAimingSocketName = TEXT("RootAiming_Socket");

public:
	FORCEINLINE bool IsScoping() const { return bIsScoping; }
	FORCEINLINE bool IsFlying() const { return bIsFlying; }
	FORCEINLINE bool IsTakeOffDelay() const { return bTakeOffDelay; }
	FORCEINLINE bool IsHoldingJump() const { return bHoldingJump; }
	FORCEINLINE bool IsMeleeAttacking() const { return bMeleeAttacking; }

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* ReviveMontage;

public:

	UFUNCTION()
	void DealtDamage(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void HitSpecialAttack(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void HitRangedAttack(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void HitMeleeAttack(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void CheckIVBag();

	UFUNCTION()
	void CheckTaser(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void CheckHardhat();

	UFUNCTION()
	void CheckNails(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void CheckRadio(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void CheckRadioDelay(AREnemyBase* hitCharacter);

	UFUNCTION(Server, Reliable)
	void CheckFriendShipBracelet();

	FTimerHandle FriendshipBraceletTimer;

	UFUNCTION()
	void HealingRadiusEffect(TSubclassOf<UGameplayEffect> healingEffect, bool IVBag);

	UFUNCTION(Server, Reliable)
	void LaunchBozo(FVector launchVelocity);

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> IVBagEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> FriendShipEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> LifestealEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> NailsEfffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> RadioEfffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> TaserEffect;

	FTimerHandle RadioDelayTimer;

private:
	bool bHasDied;

	void LevelUp(int carryOverEXP);

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	class URAttackingBoxComponent* AttackingBoxComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	class URPushBoxComponent* PushingBoxComponent;

	void PlayMontage(UAnimMontage* MontageToPlay);

	void StartDeath();
	void DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);

	void TaserTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);


	UPROPERTY(EditDefaultsOnly, Category = "Damaged")
	UAnimMontage* FlinchMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Damaged")
	UAnimMontage* AirFlinchMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TSubclassOf<UGameplayEffect> DeathEffect;

	void ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void ScopingTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsScoping;

	void FlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void FlyingTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsFlying;

	void TakeOffDelayTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void TakeOffDelayTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bTakeOffDelay;

	void HoldingJumpTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void HoldingJumpTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bHoldingJump;

	void MeleeAttackingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void MeleeAttackingTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bMeleeAttacking;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay Ability")
	URAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Player Level Up Ability")
	TSubclassOf<UGameplayEffect> LevelUpEffect;

	UPROPERTY(Transient)
	URAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HealthBarWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHealthBar> HealthBarClass;

	UPROPERTY()
	class UHealthBar* HealthBar;

	void LevelUpdated(const FOnAttributeChangeData& ChangeData);
	void ExpUpdated(const FOnAttributeChangeData& ChangeData);
	void NextLevelExpUpdated(const FOnAttributeChangeData& ChangeData);

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxHealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MovementSpeedUpdated(const FOnAttributeChangeData& ChangeData);
	void GravityUpdated(const FOnAttributeChangeData& ChangeData);
	void ForwardSpeedUpdated(const FOnAttributeChangeData& ChangeData);

	FHitResult hitResult;

public:

	UFUNCTION(NetMulticast, Reliable)
	void ServerPlayAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Unreliable)
	void ClientPlayAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Unreliable)
	void ClientStopAnimMontage(UAnimMontage* montage);

	UFUNCTION()
	AActor* GetTarget();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void UpdateAITarget(AActor* newTargetActor);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void ClientHitScanResult(AActor* hitActor, FVector start, FVector end, bool enemy);

private:

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(Replicated)
	AActor* AITarget;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

public:
	UPROPERTY(Replicated)
	int AILevel;

	UFUNCTION()
	void LevelUpUpgrade(int level, bool setLevel);

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<UGameplayEffect> LevelupUpgradeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float HealthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float MeleeStrengthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float RangedStrengthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float SpeicalStrengthOnLevelUp = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float UltimateStrengthOnLevelUp = 1;

};