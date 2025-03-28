#include "Actors/RProjectileBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"

#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"
#include "Framework/EOSActionGameState.h"

#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Enemy/REnemyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ARProjectileBase::ARProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ProjMesh = CreateDefaultSubobject<UStaticMeshComponent>("Projectile Mesh");
	ProjMesh->SetupAttachment(GetRootComponent());

	RootComponent = ProjMesh;

	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	NiagaraEffect->SetupAttachment(ProjMesh);

	// sphere radius
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(GetRootComponent());
	SphereCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));


	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->SetUpdatedComponent(ProjMesh);

	ProjectileComponent->InitialSpeed = initalSpeed;
	ProjectileComponent->MaxSpeed = maxSpeed;
}

// Called when the game starts or when spawned
void ARProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ARProjectileBase::OnOverlapBegin);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &ARProjectileBase::OnOverlapEnd);

	if (bHasPhysics)
	{
		ProjectileComponent->ProjectileGravityScale = bGravityScale;
		ProjMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		ProjectileComponent->ProjectileGravityScale = 0.0f;
		ProjMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (NiagaraEffect && !NiagaraEffect->IsActive())
	{
		NiagaraEffect->Activate();
	}

	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, this, &ARProjectileBase::DestroySelf, lifeTime, false);
}

void ARProjectileBase::DestroySelf()
{
	if (IsValid(this)) // Ensure the actor is valid before destroying it
	{
		Destroy();
	}
}

// Called every frame
void ARProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldAccelerate == false)
		return;

	// Get the current velocity
	FVector CurrentVelocity = ProjectileComponent->Velocity;

	// Normalize the velocity to get the direction
	FVector Direction = CurrentVelocity.GetSafeNormal();

	// Calculate the force or impulse
	FVector Force = Direction * accelerationStrength;
	ProjectileComponent->Velocity += Force;
}

void ARProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Error, TEXT("%s Hit Target"), *OverlappedComponent->GetName());
	ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(OtherActor);
	if (!hitCharacter)
	{
		return;
	}

	if (!OwnedPlayer)
		return;

	if (!OwnedPlayer->HasAuthority())
		return;

	bool isEnemy = false;

	AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitCharacter);
	if (hitEnemy)
		isEnemy = true;

	OnHitCharacter.Broadcast(OwnedPlayer, hitCharacter, isEnemy, hitCharacters);

	hitCharacters++;

	if (bDestroyOnhit)
	{
		DestroySelf();
	}
}

void ARProjectileBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AREnemyBase* hitEnemy = Cast<AREnemyBase>(OtherActor);
	if (!hitEnemy)
	{
		return;
	}


}

void ARProjectileBase::InitOwningCharacter(ARCharacterBase* owningCharacter)
{
	OwnedPlayer = owningCharacter;
}
