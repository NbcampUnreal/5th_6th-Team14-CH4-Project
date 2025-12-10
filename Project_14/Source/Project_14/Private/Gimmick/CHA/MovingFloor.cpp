#include "Gimmick/CHA/MovingFloor.h"
#include "Components/StaticMeshComponent.h"

AMovingFloor::AMovingFloor()
{
    PrimaryActorTick.bCanEverTick = true;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
    SetRootComponent(FloorMesh);
}

void AMovingFloor::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
    EndLocation = StartLocation + TargetOffset;

    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] BeginPlay: %s, Start=%s, End=%s"),
        *GetName(), *StartLocation.ToString(), *EndLocation.ToString());
}

void AMovingFloor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsMoving)
    {
        return;
    }

    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveTime, 0.f, 1.f);

    const FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
    SetActorLocation(NewLocation);

    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] Tick: %s, Alpha=%.2f, NewLocation=%s"),
        *GetName(), Alpha, *NewLocation.ToString());

    if (Alpha >= 1.f)
    {
        bIsMoving = false;
        UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] �̵� �Ϸ�: %s"), *GetName());
    }
}

void AMovingFloor::StartMove()
{
    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] StartMove ȣ��: %s"), *GetName());

    if (bIsMoving)
    {
        return;
    }

    bIsMoving = true;
    ElapsedTime = 0.f;
}
