// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/RTargetActor_ChesterUltimate.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

ARTargetActor_ChesterUltimate::ARTargetActor_ChesterUltimate()
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

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereAsset.Object);
	}
	SphereMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereMesh->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;
}

void ARTargetActor_ChesterUltimate::SetTargettingRadus(float targettingRadus)
{
	TargetSphere->SetSphereRadius(targettingRadus);
}

void ARTargetActor_ChesterUltimate::SetTargettingRange(float newTargettingRange)
{
	TargettingRange = newTargettingRange;
}

void ARTargetActor_ChesterUltimate::SetOwningPlayerControler(ARPlayerController* myController)
{
	MyPlayerController = myController;
}

void ARTargetActor_ChesterUltimate::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);
	FHitResult playerView = GetPlayerView();
	if (playerView.bBlockingHit)
	{
		if (MyPlayerController == nullptr)
			return;

		FVector viewLoc = MyPlayerController->GetPawn()->GetActorLocation();

		FVector hitPos = playerView.ImpactPoint;
		FVector adjustedEnd = hitPos + ((hitPos - viewLoc) * 0.5f);

		FVector Direction = (adjustedEnd - viewLoc);
		FVector cylinderLocation = (viewLoc + (adjustedEnd)) * 0.5f;

		float Length = Direction.Size();
		FRotator cylinderRotation = UKismetMathLibrary::MakeRotFromZ((hitPos - viewLoc));

		float cylinderHeight = Length;

		SetActorLocation(cylinderLocation);
		SetActorRotation(cylinderRotation);

		CylinderMesh->SetVisibility(true, true);
		SphereMesh->SetVisibility(true, true);

		FVector Scale = FVector(0.2f, 0.2f, Length / 100.0f);

		CylinderMesh->SetRelativeScale3D(Scale);
	}
}

bool ARTargetActor_ChesterUltimate::IsConfirmTargetingAllowed()
{
	return GetPlayerView().bBlockingHit;
}

void ARTargetActor_ChesterUltimate::ConfirmTargetingAndContinue()
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

FHitResult ARTargetActor_ChesterUltimate::GetPlayerView() const
{
	FHitResult hitResult;
	if (MyPlayerController)
	{
		FVector viewLoc;
		FRotator viewRot;

		MyPlayerController->GetPlayerViewPoint(viewLoc, viewRot);
		FVector traceEnd = viewLoc + viewRot.Vector() * TargettingRange;

		GetWorld()->LineTraceSingleByChannel(hitResult, viewLoc, traceEnd, ECC_Visibility);
		if (!hitResult.bBlockingHit)
		{
			GetWorld()->LineTraceSingleByChannel(hitResult, traceEnd, traceEnd + FVector::DownVector * TargettingRange, ECC_Visibility);
		}
	}
	return hitResult;
}
