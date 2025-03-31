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

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* MeleeIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText MeleeText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* RangedIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText RangedText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* SpecialIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText SpecialText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* UltimateIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText UltimateText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText PassiveText;

	UPROPERTY(EditDefaultsOnly, Category = "Character Piece")
	TSubclassOf<class ACharacter> CharacterClass;

	USkeletalMesh* GetMesh() const;
};
