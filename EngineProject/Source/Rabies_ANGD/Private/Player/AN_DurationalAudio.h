// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DurationalAudio.generated.h"

/**
 * 
 */
UCLASS()
class UAN_DurationalAudio : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
private:
	UPROPERTY(EditAnywhere, Category = "Audio")
	bool bShouldPlay = true;
};
