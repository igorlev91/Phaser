// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/RBoltHead_Actor.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Framework/EOSActionGameState.h"

// Sets default values
ARBoltHead_Actor::ARBoltHead_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	SetRootComponent(HeadMesh); // optional, or attach to another component
}

// Called when the game starts or when spawned
void ARBoltHead_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARBoltHead_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARBoltHead_Actor::ServerPlay_Head_AnimMontage_Implementation(UAnimMontage* montage, float animSpeedScale)
{
    if (HeadMesh && montage)
    {
        UAnimInstance* AnimInstance = HeadMesh->GetAnimInstance();

        if (AnimInstance && AnimInstance->Montage_IsPlaying(montage) == false)
        {

            AnimInstance->Montage_Play(montage, animSpeedScale);
        }
    }
}

USkeletalMeshComponent* ARBoltHead_Actor::GetHeadMesh() const
{
	return HeadMesh;
}
