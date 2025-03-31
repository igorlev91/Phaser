// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyState_Attack.generated.h"

/**
 * 
 */
UCLASS()
class UAnimNotifyState_Attack : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Attacking")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Attacking")
	bool bIsPush;

	UPROPERTY()
	class URAttackingBoxComponent* AttackingBoxComponent;

	UPROPERTY()
	class URPushBoxComponent* PushBoxComponent;
};
