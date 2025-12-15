#include "Gimmick/BK/ObstacleBall.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AObstacleBall::AObstacleBall()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;                
	SetReplicateMovement(true);        

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void AObstacleBall::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Mesh->SetSimulatePhysics(true);
	}
	else
	{
		Mesh->SetSimulatePhysics(false);
	}
}
