#include "Actors/RProjectileBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/RCharacterBase.h"
#include "Player/RPlayerBase.h"

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

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->SetUpdatedComponent(ProjMesh);

	ProjectileComponent->InitialSpeed = initalSpeed;
	ProjectileComponent->MaxSpeed = maxSpeed;
}

// Called when the game starts or when spawned
void ARProjectileBase::BeginPlay()
{
	Super::BeginPlay();
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
}

// Called every frame
void ARProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARCharacterBase* hitCharacter = Cast<ARCharacterBase>(OtherActor);
	if (!hitCharacter)
	{
		return;
	}

	bool isEnemy = false;

	AREnemyBase* hitEnemy = Cast<AREnemyBase>(hitCharacter);
	if (hitEnemy)
		isEnemy = true;

	OnHitCharacter.Broadcast(hitCharacter, isEnemy, hitCharacters);

	hitCharacters++;
}

void ARProjectileBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AREnemyBase* hitEnemy = Cast<AREnemyBase>(OtherActor);
	if (!hitEnemy)
	{
		return;
	}


}
