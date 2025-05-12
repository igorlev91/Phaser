// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_AddOrRemoveLooseTag.generated.h"

/**
 * 
 */
UCLASS()
class UAnimNotify_AddOrRemoveLooseTag : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	bool bAddTag;

	virtual FString GetNotifyName_Implementation() const override;
};
