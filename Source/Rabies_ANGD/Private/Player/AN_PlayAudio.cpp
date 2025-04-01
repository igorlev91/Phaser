// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AN_PlayAudio.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

void UAN_PlayAudio::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector Start = Owner->GetActorLocation();
	FVector End = Start - FVector(0, 0, 200);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	Params.bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore{MeshComp->GetOwner()};
	
	if (UKismetSystemLibrary::LineTraceSingle(MeshComp, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true))
	{
		UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
		if (PhysMat)
		{
			USoundCue* FootstepSound = nullptr;

			switch (PhysMat->SurfaceType)
			{
			case SurfaceType1:
				FootstepSound = SnowFootstepSound;
				break;
			case SurfaceType2:
				FootstepSound = MetalFootstepSound;
				break;
			case SurfaceType3:
				FootstepSound = TrashFootstepSound;
				break;
			}
		
			if (FootstepSound)
			{
				UGameplayStatics::PlaySoundAtLocation(Owner, FootstepSound, Hit.Location);
			}
		}
	}
}
