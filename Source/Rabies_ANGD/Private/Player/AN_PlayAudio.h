// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PlayAudio.generated.h"

/**
 * 
 */
UCLASS()
class UAN_PlayAudio : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* SnowFootstepSound;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* MetalFootstepSound;
};
