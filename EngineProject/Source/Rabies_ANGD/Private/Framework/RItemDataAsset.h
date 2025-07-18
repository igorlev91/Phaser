// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RItemDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common, // yellow stat changers
	Uncommon, // green
	Rare, // blue
	Unique
};

/**
 * 
 */
UCLASS()
class URItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* ItemIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	TSubclassOf<class UGameplayEffect> ItemEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UStaticMesh* ItemPlayerMesh;

	UStaticMesh* GetMesh() const;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	EItemRarity ItemRarity;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class USoundCue* ItemAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UMaterialInstance* OutlineInstance;
	
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UMaterialInstance* OrbInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FString ItemTooltip;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FString ItemDescription;
};
