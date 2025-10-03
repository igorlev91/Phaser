// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AnimNotifyState_Attacking.h"
#include "Framework/RAttackingBoxComponent.h"
#include "Framework/RPushBoxComponent.h"

void UAnimNotifyState_Attacking::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	const AActor* OwnerActor = MeshComp->GetOwner();
	AttackingBoxComponent = OwnerActor->GetComponentByClass<URAttackingBoxComponent>();
	PushBoxComponent = OwnerActor->GetComponentByClass<URPushBoxComponent>();
	if (AttackingBoxComponent && bIsPush == false)
	{
		AttackingBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		AttackingBoxComponent->StartDetection();
	}
	else if (bIsPush && PushBoxComponent)
	{
		PushBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		PushBoxComponent->StartDetection();
	}
}

void UAnimNotifyState_Attacking::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AttackingBoxComponent && bIsPush == false)
	{
		AttackingBoxComponent->DoAttackCheck();
	}
	else if (bIsPush && PushBoxComponent)
	{
		PushBoxComponent->DoAttackCheck();
	}
}

void UAnimNotifyState_Attacking::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AttackingBoxComponent && bIsPush == false)
	{
		AttackingBoxComponent->EndDetection();
	}
	else if (bIsPush && PushBoxComponent)
	{
		PushBoxComponent->EndDetection();
	}
}
