// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntry.generated.h"

/**
 * 
 */
UCLASS()
class UCharacterEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditDefaultsOnly)
	FName IconMaterialParameterName{ "Icon" };

	UPROPERTY(EditDefaultsOnly)
	FName SaturationMaterialParameterName{ "Saturation" };

	UPROPERTY(EditDefaultsOnly)
	UTexture* DefaultImage;

	//I imagine you can use this to add in multiple cameras/cages to display the actual model of the characters through something like
	
public:
	void SetCharacterIcon(const class URCharacterDefination* characterDefinition);
};
