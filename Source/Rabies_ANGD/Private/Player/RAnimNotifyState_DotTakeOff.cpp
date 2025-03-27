// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RAnimNotifyState_DotTakeOff.h"
#include "Player/RPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "Framework/EOSPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbilities/RAbilityGenericTags.h"

#include "AbilitySystemBlueprintLibrary.h"

void URAnimNotifyState_DotTakeOff::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	Player = Cast<ARPlayerBase>(MeshComp->GetOwner());
	if (!Player) return;

	FGameplayEventData eventData;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, URAbilityGenericTags::GetApplyGravityJump(), eventData);
}

void URAnimNotifyState_DotTakeOff::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	//if (!Player) return;
}

void URAnimNotifyState_DotTakeOff::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!Player) return;

}
