// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DamagePopupActor.h"
#include "Widgets/DamagePopup.h"
#include "Actors/ItemChest.h"
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
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	DamageWidgetComp = CreateDefaultSubobject<UWidgetComponent>("Damage UI");
	RootComponent = DamageWidgetComp;
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ADamagePopupActor::TimedDestroy, 0.8f, false);
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DamageUI)
	{
		float renderLerp = FMath::Lerp(DamageUI->GetRenderOpacity(), 0, DeltaTime * 4.0f);
		DamageUI->SetRenderOpacity(renderLerp);
	}

	FVector NewLocation = FMath::Lerp(GetActorLocation(), AnimOffset, DeltaTime * 6.0f);

	SetActorLocation(NewLocation);
}

void ADamagePopupActor::SetText(int damage, FVector animOffset)
{
	Damage = damage;
	AnimOffset = animOffset;
	UpdateDamageText(damage);
}

void ADamagePopupActor::UpdateDamageText(int damage)
{
	//PingUI = Cast<UPingUI>(PingUIWidgetComp->GetUserWidgetObject());
	DamageWidgetComp->SetWidgetClass(DamageUIClass);
	DamageUI = CreateWidget<UDamagePopup>(GetWorld(), DamageWidgetComp->GetWidgetClass());
	if (DamageUI)
	{
		DamageWidgetComp->SetWidget(DamageUI);
	}

	if (DamageUI)
	{
		DamageUI->SetDamage(damage);
	}
}

void ADamagePopupActor::TimedDestroy()
{
	if (IsValid(this)) // Ensure the actor is valid before destroying it
	{
		Destroy();
	}
}

void ADamagePopupActor::OnRep_Damage()
{
	UpdateDamageText(Damage);
}

void ADamagePopupActor::OnRep_AnimOffset()
{
	
}

void ADamagePopupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ADamagePopupActor, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ADamagePopupActor, AnimOffset, COND_None, REPNOTIFY_Always);
}

