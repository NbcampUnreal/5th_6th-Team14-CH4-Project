#include "Gimmick/BK/ObstacleBall.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

AObstacleBall::AObstacleBall()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void AObstacleBall::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        FTimerHandle PhysicsDelayHandle;
        GetWorldTimerManager().SetTimer(
            PhysicsDelayHandle,
            this,
            &AObstacleBall::EnablePhysics,
            0.1f,
            false
        );
    }
}

void AObstacleBall::EnablePhysics()
{
    Mesh->SetSimulatePhysics(true);
    Mesh->WakeAllRigidBodies();
}
