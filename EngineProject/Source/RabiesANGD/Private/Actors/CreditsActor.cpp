// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CreditsActor.h"
#include "Widgets/CreditsWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SphereComponent.h"

#include "Net/UnrealNetwork.h"

#include "Actors/ItemPickup.h"
#include "Components/WidgetComponent.h"

#include "Framework/RGameMode.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ACreditsActor::ACreditsActor()
{
	PrimaryActorTick.bCanEverTick = false;


	CreditsUIWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Credits UI");
	RootComponent = CreditsUIWidgetComp;
	//PingUIWidgetComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ACreditsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACreditsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

