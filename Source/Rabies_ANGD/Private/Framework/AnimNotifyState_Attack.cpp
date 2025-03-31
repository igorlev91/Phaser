// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AnimNotifyState_Attack.h"
#include "Framework/RAttackingBoxComponent.h"
#include "Framework/RPushBoxComponent.h"


void UAnimNotifyState_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const AActor* OwnerActor = MeshComp->GetOwner();
	AttackingBoxComponent = OwnerActor->GetComponentByClass<URAttackingBoxComponent>();
	PushBoxComponent = OwnerActor->GetComponentByClass<URPushBoxComponent>();
	if (AttackingBoxComponent && bIsPush == false)
	{
		AttackingBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		AttackingBoxComponent->StartDetection();
		AttackingBoxComponent->DoAttackCheck();
		AttackingBoxComponent->EndDetection();
	}
	else if (PushBoxComponent)
	{
		PushBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		PushBoxComponent->StartDetection();
		PushBoxComponent->DoAttackCheck();
		PushBoxComponent->EndDetection();
	}
}
