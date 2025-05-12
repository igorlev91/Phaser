// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chester/RTargetActor_ChesterSpecial.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

ARTargetActor_ChesterSpecial::ARTargetActor_ChesterSpecial()
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

void ARTargetActor_ChesterSpecial::SetTargettingRadus(float targettingRadus)
{
	TargetSphere->SetSphereRadius(targettingRadus);
}

void ARTargetActor_ChesterSpecial::SetTargettingRange(float newTargettingRange)
{
	TargettingRange = newTargettingRange;
}

void ARTargetActor_ChesterSpecial::SetOwningPlayerControler(ARPlayerController* myController, bool isLine)
{
	MyPlayerController = myController;
	bIsLine = isLine;
}

void ARTargetActor_ChesterSpecial::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);
	FHitResult playerView = GetPlayerView();
	if (playerView.bBlockingHit)
	{
		if (MyPlayerController == nullptr)
			return;

		FVector viewLoc = MyPlayerController->GetPawn()->GetActorLocation();
		FVector ForwardVector = MyPlayerController->GetPawn()->GetActorForwardVector();
		FVector RightVector = MyPlayerController->GetPawn()->GetActorRightVector();
		FVector UpVector = MyPlayerController->GetPawn()->GetActorUpVector();
		viewLoc += ForwardVector * 20.0f;
		viewLoc += UpVector * -80.0f;

		FVector hitPos = playerView.ImpactPoint;
		FVector adjustedEnd = hitPos + ((hitPos - viewLoc) * 0.5f);

		FVector Direction = (adjustedEnd - viewLoc);
		FVector cylinderLocation = (viewLoc + (adjustedEnd)) * 0.5f;

		float Length = Direction.Size();
		FRotator cylinderRotation = UKismetMathLibrary::MakeRotFromZ((hitPos - viewLoc));

		float cylinderHeight = Length;

		SetActorLocation((bIsLine) ? cylinderLocation : playerView.ImpactPoint);
		SetActorRotation(cylinderRotation);

		CylinderMesh->SetVisibility(bIsLine, true);
		SphereMesh->SetVisibility(!bIsLine, true);

		FVector Scale = FVector(0.1f, 0.1f, Length / 100.0f);

		CylinderMesh->SetRelativeScale3D(Scale);
	}
}

bool ARTargetActor_ChesterSpecial::IsConfirmTargetingAllowed()
{
	return GetPlayerView().bBlockingHit;
}

void ARTargetActor_ChesterSpecial::ConfirmTargetingAndContinue()
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

FHitResult ARTargetActor_ChesterSpecial::GetPlayerView() const
{
	FHitResult hitResult;
	if (MyPlayerController)
	{
		FVector viewLoc;
		FRotator viewRot;

		MyPlayerController->GetPlayerViewPoint(viewLoc, viewRot);
		ACharacter* OwnerCharacter = Cast<ACharacter>(MyPlayerController->GetPawn());
		if (OwnerCharacter != nullptr)
		{
			FVector currentVelocity = OwnerCharacter->GetVelocity();
			FVector forwardDir = OwnerCharacter->GetActorForwardVector();

			float forwardSpeed = FVector::DotProduct(currentVelocity, forwardDir);

			//UE_LOG(LogTemp, Error, TEXT("Forward Speed: %f"), forwardSpeed);
			//UE_LOG(LogTemp, Error, TEXT("Velocity Length: %f"), currentVelocity.Length());

			if (forwardSpeed <= -50)
				forwardSpeed = -300.0f;

			FVector upDir = OwnerCharacter->GetActorUpVector();
			FVector finalPoint = viewLoc + (forwardDir * 0.7f * (forwardSpeed + 600.0f)) + (upDir * -80.0f); // Use forwardSpeed

			hitResult.ImpactPoint = finalPoint;
			hitResult.Location = finalPoint;
			hitResult.TraceStart = viewLoc;
			hitResult.TraceEnd = finalPoint;
			hitResult.bBlockingHit = true;
		}
	}
	return hitResult;
}
