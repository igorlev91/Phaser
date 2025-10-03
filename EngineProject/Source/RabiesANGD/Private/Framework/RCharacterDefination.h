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

protected:
	//virtual void BeginPlay() override;

private:
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* MeleeIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UFileMediaSource* MeleeMediaSource;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText MeleeText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* RangedIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UFileMediaSource* RangedMediaSource;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText RangedText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* SpecialIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UFileMediaSource* SpecialMediaSource;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText SpecialText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* UltimateIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UFileMediaSource* UltimateMediaSource;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText UltimateText;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture* PassiveIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	class UFileMediaSource* PassiveMediaSource;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText PassiveText;

	UPROPERTY(EditDefaultsOnly, Category = "Character Piece")
	TSubclassOf<class ACharacter> CharacterClass;

	USkeletalMesh* GetMesh() const;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* PickedSoundEffect;


	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool BigStick;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool SpareBatteries;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool AmmoBox;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool LaserSight;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool OldStopwatch;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool SpeedPills;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Energycell;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool ElectronicCharger;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool CannedFood;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool IVBag;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool HardHat;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool UsedArmor;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Antidote;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool SprayBottle;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool BloodBag;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Friendshipbracelet;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Taser;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool BagofNails;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Radio;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool FuelAndCoolant;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool Heater;

	UPROPERTY(EditDefaultsOnly, Category = "Best Items")
	bool RodofUranium;
};
