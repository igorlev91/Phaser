// Fill out your copyright notice in the Description page of Project Settings.


#include "Targeting/RTargetActor_DotSpecial.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Player/RPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

ARTargetActor_DotSpecial::ARTargetActor_DotSpecial()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	TargetSphere = CreateDefaultSubobject<USphereComponent>("TargetSphere");
	TargetSphere->SetupAttachment(GetRootComponent());

	TargetSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TargetSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TargetDecal = CreateDefaultSubobject<UDecalComponent>("TargetDecal");
	TargetDecal->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;
}

void ARTargetActor_DotSpecial::SetTargettingRadus(float targettingRadus)
{
	TargetSphere->SetSphereRadius(targettingRadus);
	TargetDecal->DecalSize = FVector(targettingRadus);
}

void ARTargetActor_DotSpecial::SetTargettingRange(float newTargettingRange)
{
	TargettingRange = newTargettingRange;
}

void ARTargetActor_DotSpecial::SetOwningPlayerControler(class ARPlayerController* myController)
{
	MyPlayerController = myController;
}

void ARTargetActor_DotSpecial::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);
	FHitResult playerView = GetPlayerView();
	if (playerView.bBlockingHit)
	{
		SetActorLocation(playerView.ImpactPoint);
	}
}

bool ARTargetActor_DotSpecial::IsConfirmTargetingAllowed()
{
	return GetPlayerView().bBlockingHit;
}

void ARTargetActor_DotSpecial::ConfirmTargetingAndContinue()
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

FHitResult ARTargetActor_DotSpecial::GetPlayerView() const
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
