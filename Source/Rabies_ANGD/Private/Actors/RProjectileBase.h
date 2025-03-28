// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RProjectileBase.generated.h"

DECLARE_MULTICAST_DELEGATE_FourParams(FOnHitCharacter, class ARCharacterBase*, /*Using character*/ class ARCharacterBase* /*Hit Target*/, bool /* isEnemy */, int /* #target hit */);

UCLASS()
class ARProjectileBase : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "ChestDetail")
	class USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere, Category = "ProjectileStats")
	class UStaticMeshComponent* ProjMesh;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	class UNiagaraComponent* NiagaraEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	bool bHasPhysics;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	float bGravityScale;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	float initalSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	float maxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	float accelerationStrength;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	bool bShouldAccelerate;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	bool bDestroyOnhit;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	float lifeTime;

	int hitCharacters = 0;

	FTimerHandle DestroyHandle;
	
public:	
	FOnHitCharacter OnHitCharacter;
	// Sets default values for this actor's properties
	ARProjectileBase();

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileStats")
	class UProjectileMovementComponent* ProjectileComponent;

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Chest Detail")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "ProjectileStats")
	void InitOwningCharacter(class ARCharacterBase* owningCharacter);

	class ARCharacterBase* OwnedPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DestroySelf();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
