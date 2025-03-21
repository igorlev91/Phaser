// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Framework/RCharacterDefination.h"

void UCharacterEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	URCharacterDefination* characterDefinition = Cast<URCharacterDefination>(ListItemObject);
	if (characterDefinition)
	{
		IconImage->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParameterName, DefaultImage);
	}
}

void UCharacterEntry::SetCharacterIcon(const URCharacterDefination* characterDefinition)
{
	if (IconImage == nullptr)
	{
		return;
	}

	if (characterDefinition == nullptr)
	{
		return;
	}

	if (characterDefinition->CharacterIcon == nullptr)
	{
		return;
	}

	IconImage->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParameterName, characterDefinition->CharacterIcon);
}
