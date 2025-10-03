// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RAttackingBoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/EOSActionGameState.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Actors/ItemChest.h"
#include "Kismet/GameplayStatics.h"
#include "DisplayDebugHelpers.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Player/RPlayerBase.h"
#include "Enemy/REnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilities/RAttributeSet.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"

void URAttackingBoxComponent::StartDetection()
{
	AlreadyDetectedActors.Empty();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void URAttackingBoxComponent::DoAttackCheck()
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
			if (bEnemy)
			{
				ARPlayerBase* overlappedPlayer = Cast<ARPlayerBase>(result.GetActor());
				if (overlappedPlayer)
				{
					if (overlappedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetImmuneTag())
						|| overlappedPlayer->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) continue;

					TargetFound(overlappedPlayer);
				}
			}
			else
			{
				AREnemyBase* overlappedEnemy = Cast<AREnemyBase>(result.GetActor());
				if (overlappedEnemy)
				{
					if (overlappedEnemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetImmuneTag())
						|| overlappedEnemy->GetAbilitySystemComponent()->HasMatchingGameplayTag(URAbilityGenericTags::GetDeadTag())) continue;

					TargetFound(overlappedEnemy);
				}
			}
		}
	}

	//DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), FColor::Red, false, 1.f);
}

void URAttackingBoxComponent::EndDetection()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AlreadyDetectedActors.Empty();
}

void URAttackingBoxComponent::TargetFound(AActor* OverlappedActor)
{
	if (!GetOwner()->HasAuthority()) return;
	if (OverlappedActor == GetOwner()) return;
	if (AlreadyDetectedActors.Contains(OverlappedActor)) return;

	AlreadyDetectedActors.Add(OverlappedActor);
	FGameplayEventData data;
	data.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(OverlappedActor);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), URAbilityGenericTags::GetGenericTargetAquiredTag(), data);
}
