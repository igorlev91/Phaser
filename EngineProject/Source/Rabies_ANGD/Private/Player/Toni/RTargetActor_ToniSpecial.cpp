// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Toni/RTargetActor_ToniSpecial.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

ARTargetActor_ToniSpecial::ARTargetActor_ToniSpecial()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	TargetSphere = CreateDefaultSubobject<USphereComponent>("TargetSphere");
	TargetSphere->SetupAttachment(GetRootComponent());

	TargetSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Create Cylinder Mesh
	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderAsset.Succeeded())
	{
		CylinderMesh->SetStaticMesh(CylinderAsset.Object);
	}
	CylinderMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CylinderMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CylinderMesh->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;
}

void ARTargetActor_ToniSpecial::SetTargettingRadus(float targettingRadus)
{
	TargetSphere->SetSphereRadius(targettingRadus);
}

void ARTargetActor_ToniSpecial::SetOwningPlayerControler(ARPlayerController* myController, float yawAdjustment)
{
	MyPlayerController = myController;
	YawAdjustment = yawAdjustment;
}

void ARTargetActor_ToniSpecial::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);
	SetMeshPosition(GetPlayerView(YawAdjustment));
}

bool ARTargetActor_ToniSpecial::IsConfirmTargetingAllowed()
{
	return GetPlayerView(0).bBlockingHit;
}

void ARTargetActor_ToniSpecial::ConfirmTargetingAndContinue()
{
	if (MyPlayerController == nullptr)
		return;

	TSet<AActor*> currentActorsInRange;
	TargetSphere->GetOverlappingActors(currentActorsInRange, APawn::StaticClass());

	TArray<AActor*> targets;
	for (AActor* actorInRange : currentActorsInRange)
	{
		if (actorInRange != MyPlayerController->GetPawn())
			targets.Add(actorInRange);
	}

	FGameplayAbilityTargetDataHandle targetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(targets, false);

	FHitResult centerLocationHitResult;
	centerLocationHitResult.ImpactPoint = GetActorLocation();

	FGameplayAbilityTargetData_SingleTargetHit* targetCenterLocationData = new FGameplayAbilityTargetData_SingleTargetHit{ centerLocationHitResult };
	targetDataHandle.Add(targetCenterLocationData);
	TargetDataReadyDelegate.Broadcast(targetDataHandle);
}


void ARTargetActor_ToniSpecial::SetMeshPosition(FHitResult givenPlayerView)
{
	if (givenPlayerView.bBlockingHit)
	{
		if (MyPlayerController == nullptr)
			return;

		FVector viewLoc = MyPlayerController->GetPawn()->GetActorLocation();
		FVector ForwardVector = MyPlayerController->GetPawn()->GetActorForwardVector();
		FVector RightVector = MyPlayerController->GetPawn()->GetActorRightVector();
		FVector UpVector = MyPlayerController->GetPawn()->GetActorUpVector();
		viewLoc += ForwardVector * 10.0f;
		//viewLoc += RightVector * -35.0f;
		viewLoc += UpVector * -42.0f;

		FVector hitPos = givenPlayerView.ImpactPoint;
		FVector adjustedEnd = hitPos + ((hitPos - viewLoc) * 0.5f);

		FVector Direction = (adjustedEnd - viewLoc);
		FVector cylinderLocation = (viewLoc + (adjustedEnd)) * 0.5f;

		float Length = Direction.Size();
		FRotator cylinderRotation = UKismetMathLibrary::MakeRotFromZ((hitPos - viewLoc));

		float cylinderHeight = Length;

		SetActorLocation(cylinderLocation);
		SetActorRotation(cylinderRotation);

		CylinderMesh->SetVisibility(true, true);

		FVector Scale = FVector(0.03f, 0.03f, Length / 100.0f);

		CylinderMesh->SetRelativeScale3D(Scale);
	}
}

FHitResult ARTargetActor_ToniSpecial::GetPlayerView(float yawAdjustment) const
{
	FHitResult hitResult;
	if (MyPlayerController)
	{
		FVector viewLoc;
		FRotator viewRot;

		MyPlayerController->GetPlayerViewPoint(viewLoc, viewRot);
		viewRot.Yaw -= yawAdjustment;
		viewRot.Pitch += 1;

		// Move directly forward from the camera to max range
		FVector forwardDir = viewRot.Vector();
		FVector finalPoint = viewLoc + forwardDir * 4000.0f;

		hitResult.ImpactPoint = finalPoint;
		hitResult.Location = finalPoint;
		hitResult.TraceStart = viewLoc;
		hitResult.TraceEnd = finalPoint;
		hitResult.bBlockingHit = true;
	}
	return hitResult;
}
