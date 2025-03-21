// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RCharacterDefination.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

USkeletalMesh* URCharacterDefination::GetMesh() const
{
	if (CharacterClass)
	{
		ACharacter* characterCDO = Cast<ACharacter>(CharacterClass->GetDefaultObject());
		if (characterCDO)
		{
			return characterCDO->GetMesh()->GetSkeletalMeshAsset();
		}
	}

	return nullptr;
}
