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

UCLASS()
class ARCharacterBase : public ACharacter, public IAbilitySystemInterface, /*public IRGameplayCueInterface,*/ public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FOnDeadStatusChanged OnDeadStatusChanged;

	// Sets default values for this character's properties
	ARCharacterBase();

	void SetupAbilitySystemComponent();
	void InitAttributes();
	void InitAbilities();
	//virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:

	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	void InitStatusHUD();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	FORCEINLINE bool IsScoping() const { return bIsScoping; }
	FORCEINLINE bool IsFlying() const { return bIsFlying; }

private:

	void ScopingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void ScopingTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsScoping;

	void FlyingTagChanged(const FGameplayTag TagChanged, int32 NewStackCount);
	virtual void FlyingTagChanged(bool bNewIsAiming) {/*empty in base*/ };
	bool bIsFlying;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay Ability")
	URAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	URAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HealthBarWidgetComp;

	UPROPERTY()
	class UHealthBar* HealthBar;

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MaxHealthUpdated(const FOnAttributeChangeData& ChangeData);
	void MovementSpeedUpdated(const FOnAttributeChangeData& ChangeData);
	void GravityUpdated(const FOnAttributeChangeData& ChangeData);

public:
	UFUNCTION(NetMulticast, Unreliable)
	void ClientPlayAnimMontage(UAnimMontage* montage);

	UFUNCTION(NetMulticast, Unreliable)
	void ClientStopAnimMontage(UAnimMontage* montage);


private:

	//UPROPERTY(Replicated)
	//FGenericTeamId TeamId;

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // need this when doing Replicated things

};