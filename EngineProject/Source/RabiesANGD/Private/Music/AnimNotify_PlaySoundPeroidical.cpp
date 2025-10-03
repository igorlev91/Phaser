// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/AnimNotify_PlaySoundPeroidical.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void UAnimNotify_PlaySoundPeroidical::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		OwnerAvatarActor = MeshComp->GetOwner();

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
		TArray<AActor*> ActorsToIgnore{ MeshComp->GetOwner() };

		if (UKismetSystemLibrary::LineTraceSingle(MeshComp, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true))
		{
			UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
			if (PhysMat)
			{
				SoundToPlay = nullptr;

				switch (PhysMat->SurfaceType)
				{
				case SurfaceType1:
					SoundToPlay = SnowFootstepSound;
					break;
				case SurfaceType2:
					SoundToPlay = MetalFootstepSound;
					break;
				case SurfaceType3:
					SoundToPlay = TrashFootstepSound;
					break;
				}
			}
		}

		MeshComp->GetWorld()->GetTimerManager().SetTimer(SoundPlayTimerHandle, this, &UAnimNotify_PlaySoundPeroidical::PlaySound, Interval, true);
	}
}

void UAnimNotify_PlaySoundPeroidical::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		MeshComp->GetWorld()->GetTimerManager().ClearTimer(SoundPlayTimerHandle);
	}
}

void UAnimNotify_PlaySoundPeroidical::PlaySound()
{
	if (IsValid(OwnerAvatarActor))
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerAvatarActor, SoundToPlay, OwnerAvatarActor->GetActorLocation());
	}
}
