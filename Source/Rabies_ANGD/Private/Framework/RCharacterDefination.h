// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RCharacterDefination.generated.h"

/**
 * 
 */
UCLASS()
class URCharacterDefination : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character Piece")
	TSubclassOf<class UAnimInstance> CharacterAnimationInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Character Piece")
	TSubclassOf<class ACharacter> CharacterClass;

	USkeletalMesh* GetMesh() const;
};
