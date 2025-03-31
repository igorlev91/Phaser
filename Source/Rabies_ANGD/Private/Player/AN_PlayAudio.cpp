// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AN_PlayAudio.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

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

	if (Owner->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
	{
		DrawDebugLine(Owner->GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
		DrawDebugPoint(Owner->GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Green, false, 2.0f);

		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());

		//if (Hit.PhysMaterial.IsValid())
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Valid"));
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		//}


		//UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
		//if (PhysMat)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Hitting"));
		//	//USoundCue* FootstepSound = nullptr;

		//	//switch (PhysMat->SurfaceType)
		//	//{
		//	//case SurfaceType1:
		//	//	UE_LOG(LogTemp, Warning, TEXT("Stepping on Snow"));
		//	//	FootstepSound = SnowFootstepSound;
		//	//	break;
		//	//case SurfaceType2:
		//	//	UE_LOG(LogTemp, Warning, TEXT("Stepping on Metal"));
		//	//	FootstepSound = MetalFootstepSound;
		//	//	break;
		//	//}
		//	//
		//	//if (FootstepSound)
		//	//{
		//	//	UE_LOG(LogTemp, Warning, TEXT("Hey you, play that sound"));
		//	//	UGameplayStatics::PlaySoundAtLocation(Owner, FootstepSound, Hit.Location);
		//	//}
		//}
	}
}
