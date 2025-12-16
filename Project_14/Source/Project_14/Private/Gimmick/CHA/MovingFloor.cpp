#include "Gimmick/CHA/MovingFloor.h"
#include "Components/StaticMeshComponent.h"

AMovingFloor::AMovingFloor()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    SetRootComponent(FloorMesh);
}

void AMovingFloor::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
    EndLocation = StartLocation + TargetOffset;
}

void AMovingFloor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ✅ 서버에서만 실제 이동
    if (!HasAuthority())
        return;

    if (!bIsMoving)
        return;

    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveTime, 0.f, 1.f);

    const FVector From = bForward ? StartLocation : EndLocation;
    const FVector To = bForward ? EndLocation : StartLocation;

    SetActorLocation(FMath::Lerp(From, To, Alpha));

    if (Alpha >= 1.f)
    {
        bForward = !bForward;
        ElapsedTime = 0.f;
    }
}

void AMovingFloor::StartMove()
{
    if (!HasAuthority())
        return;

    if (bIsMoving) return;
    bIsMoving = true;
    bForward = true;
    ElapsedTime = 0.f;
}

void AMovingFloor::SetMoving(bool bMove)
{
    if (!HasAuthority())
        return;

    bIsMoving = bMove;

    if (bIsMoving)
    {
        bForward = true;
        ElapsedTime = 0.f;
    }
}
