#include "Gimmick/BK/ObstacleBall.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

AObstacleBall::AObstacleBall()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    Mesh->SetNotifyRigidBodyCollision(true);
    Mesh->OnComponentHit.AddDynamic(this, &AObstacleBall::OnHit);
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

    Mesh->SetMassOverrideInKg(NAME_None, NormalMass, true);
    Mesh->WakeAllRigidBodies();
}

void AObstacleBall::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    if (!HasAuthority())
        return;

    if (NormalImpulse.Size() > PushImpulseThreshold)
    {
        Mesh->SetMassOverrideInKg(NAME_None, PushableMass, true);

        GetWorldTimerManager().ClearTimer(ResetMassTimer);
        GetWorldTimerManager().SetTimer(
            ResetMassTimer,
            [this]()
            {
                if (Mesh)
                {
                    Mesh->SetMassOverrideInKg(NAME_None, NormalMass, true);
                }
            },
            0.4f,
            false
        );
    }

}
