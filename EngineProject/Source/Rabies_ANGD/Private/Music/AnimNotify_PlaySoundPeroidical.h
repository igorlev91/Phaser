// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotify_PlaySoundPeroidical.generated.h"

/**
 * 
 */
UCLASS()
class UAnimNotify_PlaySoundPeroidical : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	//virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void PlaySound();

	FTimerHandle SoundPlayTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Timming")
	float Interval = 1.f;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* SnowFootstepSound;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* MetalFootstepSound;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* TrashFootstepSound;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	class USoundCue* SoundToPlay;

	UPROPERTY()
	class AActor* OwnerAvatarActor;
};
