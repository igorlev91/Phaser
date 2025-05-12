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
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInvisStatusChanged, bool /*bIsDead*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTaserStatusChanged, bool /*bIsTased*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int /*new level*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDealtDamage, ARCharacterBase* /*damage dealt*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageRecieved, float /*damage recieved*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnClientHitScan, AActor* /*Hit Target*/, FVector /* Start Pos */, FVector /* End Pos */, bool /*crit*/);

UCLASS()
class ARCharacterBase : public ACharacter, public IAbilitySystemInterface, /*public IRGameplayCueInterface,*/ public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FOnDeadStatusChanged OnDeadStatusChanged;

	FOnInvisStatusChanged OnInvisStatusChanged;

	FOnTaserStatusChanged OnTaserStatusChanged;

	FOnClientHitScan ClientHitScan;
	FOnLevelUp onLevelUp;

	FOnDealtDamage OnDealtDamage;
	FOnDamageRecieved OnDamageRecieved;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDecalComponent* DecalComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UMaterialInterface* DecalMaterial;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDecal(UMaterialInterface* newMaterial);

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
	FName RangedAltAttackSocketName = TEXT("RangedAlt_Socket");
	FName RootAimingSocketName = TEXT("RootAiming_Socket");

	UPROPERTY(Replicated)
	bool DoAltAiming;

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
	void ApplyItemEffectAtRandom(ARCharacterBase* hitCharacter, FGameplayAttribute effectChance, TSubclassOf<UGameplayEffect> effectToApply);

	UFUNCTION()
	void CheckHardhat();

	UFUNCTION()
	void CheckRadio(ARCharacterBase* hitCharacter);

	UFUNCTION()
	void CheckRadioDelay(AREnemyBase* hitCharacter);

	UFUNCTION()
	void CheckRadiationDelay(AREnemyBase* hitCharacter);

	UFUNCTION()
	void CheckFriendShipBracelet();

	UFUNCTION()
	void CheckRadiationDamage();

	UFUNCTION()
	void CheckFireDamage();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* RadioSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* FireSystem;

	UPROPERTY()
	class UNiagaraComponent* CurrentFire;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* HealingSelf;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* RedHealingSelf;

	FTimerHandle FriendshipBraceletTimer;
	FTimerHandle RadiationTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UMaterialInstance* RadOutlineInstance;

	bool bRadiated = false;

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

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> RadiationEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> RadiationDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> FireEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> IceEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<UGameplayEffect> FireDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Passive")
	TSubclassOf<UGameplayEffect> SpeedUpPassive;

	FTimerHandle RadioDelayTimer;
	FTimerHandle RadiationDelayTimer;
	FTimerHandle FireDelayTimer;

	FName WeakpointSocketName = TEXT("Weakpoint");

	bool bFireNext;


	UFUNCTION()
	void SetAndShowWeakpointUI(class ARCharacterBase* ScopingCharacter);

	UFUNCTION()
	void HideWeakpointUI(class ARCharacterBase* ScopingCharacter);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float WeakpointSize ;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UMaterialInstanceDynamic* DynamicMaterialInstance;  // The dynamic material instance

	UFUNCTION()
	void SetPairedPing(class APingActor* myPing);

	UFUNCTION()
	bool HasPing();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UTexture* CharacterPingIcon;

private:
	UFUNCTION()
	void ProjectDropShadow();

	UPROPERTY(Replicated)
	class APingActor* MyPing;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UStaticMeshComponent* Weapon_LeftHand;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UStaticMeshComponent* Weapon_RightHand;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName WeaponLeftSocketName = TEXT("Replace With Joint");

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName WeaponRightSocketName = TEXT("Replace With Joint");

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UCapsuleComponent* WeakpointCollider;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UWidgetComponent* WeakpointWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class UWeakpointUI> WeakpointClass;

	UPROPERTY()
	class UWeakpointUI* WeakpointUI;

	bool bHasDied;


	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UMaterial* MyMaterial;  // The base material reference in the editor

	void LevelUp(int carryOverEXP);

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	class URAttackingBoxComponent* AttackingBoxComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	class URPushBoxComponent* PushingBoxComponent;

	void PlayMontage(UAnimMontage* MontageToPlay);

	void StartDeath();
	void DeathTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);

	void TaserTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void TaserTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsTased;


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

	void InvisTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void InvisTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsInvis;


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

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_HandleFireVFX(float fireValue);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_HandleIceVFX(float iceValue);

	//UFUNCTION(NetMulticast, Reliable)
	void HandleFireVFX(const FOnAttributeChangeData& ChangeData);
	void HandleIceVFX(const FOnAttributeChangeData& ChangeData);

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

	UPROPERTY()
	class UHealthBar* HealthBar;

	UFUNCTION(NetMulticast, Reliable)
	void ServerPlayAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Reliable)
	void ServerPlay_Torso_AnimMontage(UAnimMontage* montage, float animSpeedScale);

	UFUNCTION(NetMulticast, Reliable)
	void ServerStopAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Unreliable)
	void ClientPlayAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Unreliable)
	void ClientStopAnimMontage(UAnimMontage* montage);

	UFUNCTION()
	AActor* GetTarget();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void UpdateAITarget(AActor* newTargetActor);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void ClientHitScanResult(AActor* hitActor, FVector start, FVector end, bool enemy, bool bIsCrit);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Icon")
	UTexture* CharacterIcon;

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(Replicated)
	AActor* AITarget;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

public:
	UPROPERTY(Replicated)
	float attackSpeedBeforeSpecial = 1.0f;

	UFUNCTION()
	void SetHealthBarFromAllyPerspective(FVector viewingLocation);

	UPROPERTY(Replicated)
	int AILevel;

	UFUNCTION()
	void LevelUpUpgrade(int level, bool setLevel);

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<UGameplayEffect> LevelupUpgradeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TSubclassOf<UGameplayEffect> LevelupRegenEffect;

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