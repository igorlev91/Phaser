// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RPushBoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/EOSActionGameState.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

void URPushBoxComponent::StartDetection()
{
	AlreadyDetectedActors.Empty();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void URPushBoxComponent::DoAttackCheck()
{
	TArray<FOverlapResult> outResult;
	FCollisionShape detectionShape;
	const FVector extend = GetScaledBoxExtent();
	detectionShape.SetBox(FVector3f{ (float)extend.X, (float)extend.Y, (float)extend.Z });

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->OverlapMultiByChannel(outResult, GetComponentLocation(), GetComponentQuat(), ECC_Pawn, detectionShape, queryParams))
	{
		for (const FOverlapResult& result : outResult)
		{
			AActor* overlappedActor = result.GetActor();
			if (result.GetComponent() == overlappedActor->GetRootComponent())
			{
				TargetFound(overlappedActor);
			}
		}
	}

	//DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), FColor::Red, false, 1.f);
}

void URPushBoxComponent::EndDetection()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AlreadyDetectedActors.Empty();
}

void URPushBoxComponent::TargetFound(AActor* OverlappedActor)
{
	if (!GetOwner()->HasAuthority()) return;
	if (OverlappedActor == GetOwner()) return;
	if (AlreadyDetectedActors.Contains(OverlappedActor)) return;

	AlreadyDetectedActors.Add(OverlappedActor);
	FGameplayEventData data;
	data.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OverlappedActor);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), URAbilityGenericTags::GetExpTag(), data);
}
