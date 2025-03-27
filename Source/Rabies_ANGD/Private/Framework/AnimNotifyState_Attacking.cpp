// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AnimNotifyState_Attacking.h"
#include "Framework/RAttackingBoxComponent.h"

void UAnimNotifyState_Attacking::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	const AActor* OwnerActor = MeshComp->GetOwner();
	AttackingBoxComponent = OwnerActor->GetComponentByClass<URAttackingBoxComponent>();
	if (AttackingBoxComponent)
	{
		AttackingBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		AttackingBoxComponent->StartDetection();
	}
}

void UAnimNotifyState_Attacking::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (AttackingBoxComponent)
	{
		AttackingBoxComponent->DoAttackCheck();
	}
}

void UAnimNotifyState_Attacking::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AttackingBoxComponent)
	{
		AttackingBoxComponent->EndDetection();
	}
}
