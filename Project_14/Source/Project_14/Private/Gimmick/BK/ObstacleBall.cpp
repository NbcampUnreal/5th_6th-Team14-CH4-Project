#include "Gimmick/BK/ObstacleBall.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"

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
    if (!HasAuthority())
        return;

    Mesh->SetSimulatePhysics(true);
    Mesh->SetMassOverrideInKg(NAME_None, NormalMass);
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

    if (!OtherActor)
        return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (!Player)
        return;

    float BounceStrength = 0.f;

    switch (Player->CharacterType)
    {
    case ECharacterType::WeakJump:
        BounceStrength = WeakJumpBounceStrength;
        break;

    case ECharacterType::StrongPush:
        BounceStrength = StrongPushBounceStrength;
        break;

    default:
        return;
    }

    FVector BackwardDir = -Player->GetActorForwardVector();
    BackwardDir.Z = 0.4f;
    BackwardDir.Normalize();

    Player->LaunchCharacter(
        BackwardDir * BounceStrength,
        true,  
        true    
    );
}
