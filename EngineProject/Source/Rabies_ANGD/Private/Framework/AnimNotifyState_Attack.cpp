// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/AnimNotifyState_Attack.h"
#include "Framework/RAttackingBoxComponent.h"
#include "Framework/RPushBoxComponent.h"
#include "CollisionQueryParams.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemy/REnemyBase.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UAnimNotifyState_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const AActor* OwnerActor = MeshComp->GetOwner();
	AttackingBoxComponent = OwnerActor->GetComponentByClass<URAttackingBoxComponent>();
	PushBoxComponent = OwnerActor->GetComponentByClass<URPushBoxComponent>();
	if (AttackingBoxComponent && bIsPush == false)
	{
		AttackingBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		AttackingBoxComponent->StartDetection();
		AttackingBoxComponent->DoAttackCheck();
		AttackingBoxComponent->EndDetection();

		DoMeleeParticle(OwnerActor, MeshComp);
	}
	else if (PushBoxComponent)
	{
		PushBoxComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
		PushBoxComponent->StartDetection();
		PushBoxComponent->DoAttackCheck();
		PushBoxComponent->EndDetection();

		DoMeleeParticle(OwnerActor, MeshComp);
	}
}

void UAnimNotifyState_Attack::DoMeleeParticle(const AActor* OwnerActor, USkeletalMeshComponent* MeshComp)
{
	float SweepRadius = 50.0f;

	FVector StartLocation = MeshComp->GetSocketLocation(AttachSocket);
	FVector EndLocation = MeshComp->GetSocketLocation(AttachSocketEnd);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;
	ObjectTypeQueries.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypeQueries.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());
	//EDrawDebugTrace::Type DebugType = EDrawDebugTrace::ForDuration;
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

	TArray<FHitResult> OutHits;
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLocation, EndLocation, SweepRadius, ObjectTypeQueries, false, ActorsToIgnore, DebugType, OutHits, true);

	if (bHit)
	{
		TArray<AREnemyBase*> enemiesHit;

		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			FName HitBoneName = Hit.BoneName;
			FVector HitPoint = Hit.ImpactPoint;
			FVector HitNormal = Hit.ImpactNormal;

			int randomHit = FMath::RandRange(0, HitParticles.Num() - 1);
			if (HitParticles[randomHit])
			{
				UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp, HitParticles[randomHit], HitPoint, FRotator::ZeroRotator, FVector(1.0f), true, true);
			}

			if (RobotGiblets == nullptr)
				return;

			AREnemyBase* enemy = Cast<AREnemyBase>(HitActor);
			if (enemy == nullptr)
				return;

			if (enemiesHit.Contains(enemy))
				return;
			
			enemiesHit.Add(enemy);

			UNiagaraComponent* SpawnSystemAttached = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp, RobotGiblets, HitPoint, FRotator::ZeroRotator, FVector(1.0f), true, true);
			if (SpawnSystemAttached)
			{
				//UE_LOG(LogTemp, Error, TEXT("creating giblets"));
				SpawnSystemAttached->SetVectorParameter(FName("Direction"), HitNormal);
				SpawnSystemAttached->SetIntParameter(FName("Amount"), enemy->GibletCount);
			}
		}
	}
}
