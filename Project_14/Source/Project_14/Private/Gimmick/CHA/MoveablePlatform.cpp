// MoveablePlatform.cpp

#include "Gimmick/CHA/MoveablePlatform.h"
#include "Components/StaticMeshComponent.h"

AMoveablePlatform::AMoveablePlatform()
{
    // 발판은 Tick 안 씀
    PrimaryActorTick.bCanEverTick = false;

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);

    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMoveablePlatform::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
}

void AMoveablePlatform::MoveStep(const FVector& Direction, float DeltaTime)
{
    if (MoveSpeed <= 0.f || DeltaTime <= 0.f)
    {
        return;
    }

    FVector Dir = Direction;
    Dir.Z = 0.f;

    if (Dir.IsNearlyZero())
    {
        return;
    }

    Dir.Normalize();

    // 이번 프레임 이동량
    const FVector DeltaMove = Dir * MoveSpeed * DeltaTime;

    FVector ProposedLocation = GetActorLocation() + DeltaMove;
    FVector ProposedOffset = ProposedLocation - StartLocation;

    // 시작 위치 기준으로 오프셋 클램프
    FVector ClampedOffset = ProposedOffset;
    ClampedOffset.X = FMath::Clamp(ClampedOffset.X, MinOffset.X, MaxOffset.X);
    ClampedOffset.Y = FMath::Clamp(ClampedOffset.Y, MinOffset.Y, MaxOffset.Y);
    ClampedOffset.Z = FMath::Clamp(ClampedOffset.Z, MinOffset.Z, MaxOffset.Z);

    // 경계 안쪽으로 위치 고정
    SetActorLocation(StartLocation + ClampedOffset);
}
