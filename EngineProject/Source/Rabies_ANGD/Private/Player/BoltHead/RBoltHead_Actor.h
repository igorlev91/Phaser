// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RBoltHead_Actor.generated.h"

UCLASS()
class ARBoltHead_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARBoltHead_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class ARPlayerBase* owningPlayer;

	UFUNCTION(NetMulticast, Reliable)
	void ServerPlay_Head_AnimMontage(UAnimMontage* montage, float animSpeedScale);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* HeadMesh;

	UFUNCTION(BlueprintCallable, Category = "BoltHead")
	USkeletalMeshComponent* GetHeadMesh() const;
};
