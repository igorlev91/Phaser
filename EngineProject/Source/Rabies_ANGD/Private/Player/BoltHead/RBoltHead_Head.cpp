// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BoltHead/RBoltHead_Head.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerController.h"
#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

#include "GameplayAbilities/RAbilitySystemComponent.h"

#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

URBoltHead_Head::URBoltHead_Head()
{
	//bReplicates = true;
}

void URBoltHead_Head::WeeWoo(ARPlayerBase* damagedPlayer, float movementSpeed, float ReviveSpeed)
{
    // Only proceed if the player is actually dead
    if (!damagedPlayer || !damagedPlayer->GetAbilitySystemComponent() ||
        !damagedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag()))
    {
        return;
    }

    DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    FVector CurrentLocation = GetComponentLocation();
    FVector TargetLocation = damagedPlayer->GetActorLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), movementSpeed * 0.01f);
    SetWorldLocation(NewLocation);

    // Recurse on next tick via timer manager
    FinishedHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(
        this, &URBoltHead_Head::WeeWoo, damagedPlayer, movementSpeed, ReviveSpeed));
}