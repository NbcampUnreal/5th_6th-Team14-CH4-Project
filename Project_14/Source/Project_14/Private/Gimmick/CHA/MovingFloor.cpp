// MovingFloor.cpp

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

    // 현재 액터 위치를 기준으로 시작/끝 위치 계산
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

    // 진행 비율(0 ~ 1)
    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveTime, 0.f, 1.f);

    // 방향에 따라 시작/끝 위치를 바꿔서 Lerp
    const FVector From = bForward ? StartLocation : EndLocation;
    const FVector To = bForward ? EndLocation : StartLocation;

    const FVector NewLocation = FMath::Lerp(From, To, Alpha);
    SetActorLocation(NewLocation);

    UE_LOG(LogTemp, Verbose, TEXT("[MovingFloor] Tick: %s, Alpha=%.2f, NewLocation=%s"),
        *GetName(), Alpha, *NewLocation.ToString());

    // 한쪽 끝까지 도달하면 방향 전환 + 시간 리셋 → 계속 왕복
    if (Alpha >= 1.f)
    {
        bForward = !bForward;
        ElapsedTime = 0.f;

        UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] 방향 전환: %s, bForward=%s"),
            *GetName(), bForward ? TEXT("true") : TEXT("false"));
    }
}

void AMovingFloor::StartMove()
{
    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] StartMove 호출: %s"), *GetName());

    if (bIsMoving)
    {
        return;
    }

    bIsMoving = true;
    bForward = true;   // 처음은 항상 Start -> End 방향부터
    ElapsedTime = 0.f;
}

void AMovingFloor::SetMoving(bool bMove)
{
    bIsMoving = bMove;

    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] SetMoving(%s): %s"),
        bMove ? TEXT("true") : TEXT("false"),
        *GetName());
}
