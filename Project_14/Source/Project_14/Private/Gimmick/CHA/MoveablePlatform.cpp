#include "Gimmick/CHA/MoveablePlatform.h"
#include "Components/StaticMeshComponent.h"

AMoveablePlatform::AMoveablePlatform()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);
}

void AMoveablePlatform::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();

    if (BoundsActor)
    {
        const FBox Box = BoundsActor->GetComponentsBoundingBox(true);
        WorldMin = Box.Min;
        WorldMax = Box.Max;
        bUseWorldBounds = true;
    }
    else
    {
        bUseWorldBounds = false;
    }
}

void AMoveablePlatform::MoveStep(const FVector& Direction, float DeltaTime)
{
    // ✅ 서버만 이동 계산
    if (!HasAuthority())
        return;

    if (Direction.IsNearlyZero())
        return;

    FVector Dir = Direction.GetClampedToMaxSize(1.f);
    FVector NewLoc = GetActorLocation() + Dir * MoveSpeed * DeltaTime;

    NewLoc.Z = StartLocation.Z;

    if (bUseWorldBounds)
    {
        NewLoc.X = FMath::Clamp(NewLoc.X, WorldMin.X + BoundsPadding, WorldMax.X - BoundsPadding);
        NewLoc.Y = FMath::Clamp(NewLoc.Y, WorldMin.Y + BoundsPadding, WorldMax.Y - BoundsPadding);
    }
    else
    {
        FVector Offset = NewLoc - StartLocation;

        Offset.X = FMath::Clamp(Offset.X, MinOffset.X, MaxOffset.X);
        Offset.Y = FMath::Clamp(Offset.Y, MinOffset.Y, MaxOffset.Y);
        Offset.Z = FMath::Clamp(Offset.Z, MinOffset.Z, MaxOffset.Z);

        NewLoc = StartLocation + Offset;
    }

    SetActorLocation(NewLoc);
}
