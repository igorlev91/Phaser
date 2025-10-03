// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/RItemDataAsset.h"

UStaticMesh* URItemDataAsset::GetMesh() const
{
	if (ItemMesh)
	{
		AActor* ActorCDO = Cast<AActor>(ItemMesh->StaticClass());
		if (ActorCDO)
		{
			UStaticMeshComponent* MeshComponent = ActorCDO->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComponent)
			{
				return MeshComponent->GetStaticMesh();
			}
		}
	}

	return nullptr;
}
