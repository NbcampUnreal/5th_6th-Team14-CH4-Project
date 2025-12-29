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

    PointA = GetActorLocation();
    PointB = PointA + TargetOffset;

    // 시작은 멈춤 상태로 두고, 세그먼트는 “현재 위치 -> 목표”로 준비만 가능
    bForward = true; // 처음 시작하면 B로 가도록
    bIsMoving = false;
    ElapsedTime = 0.f;
    SegmentDuration = 0.f;
    SegmentStart = GetActorLocation();
    SegmentEnd = PointB;
}

void AMovingFloor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ✅ 서버에서만 실제 이동
    if (!HasAuthority())
        return;

    if (!bIsMoving)
        return;

    // 혹시 남은 거리가 0이면 바로 다음 구간 세팅
    if (SegmentDuration <= KINDA_SMALL_NUMBER)
    {
        // 현재 도착 지점에서 방향 뒤집고 다음 구간 준비
        bForward = !bForward;
        SetupSegmentFromCurrent();
        return;
    }

    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / SegmentDuration, 0.f, 1.f);

    SetActorLocation(FMath::Lerp(SegmentStart, SegmentEnd, Alpha));

    if (Alpha >= 1.f)
    {
        // 한 끝점에 도착했으면 방향 뒤집고 다음 구간 시작
        bForward = !bForward;
        SetupSegmentFromCurrent();
    }
}

void AMovingFloor::SetupSegmentFromCurrent()
{
    const FVector Current = GetActorLocation();

    // 이번에 향할 목표 끝점
    const FVector Target = bForward ? PointB : PointA;

    SegmentStart = Current;
    SegmentEnd = Target;

    // 전체 구간 길이와 속도 계산(속도 일정하게 유지)
    const float FullDist = FVector::Dist(PointA, PointB);
    const float Speed = (MoveTime > 0.f) ? (FullDist / MoveTime) : 0.f;

    const float RemainDist = FVector::Dist(Current, Target);

    // 남은 거리 비례 시간(일시정지 후 재개 시 “그 자리부터 남은 만큼”만 이동)
    if (Speed <= KINDA_SMALL_NUMBER)
    {
        SegmentDuration = 0.f;
    }
    else
    {
        SegmentDuration = RemainDist / Speed;
    }

    ElapsedTime = 0.f;
}

void AMovingFloor::StartMove()
{
    // StartMove는 그냥 “이동 켜기”로 통일
    SetMoving(true);
}

void AMovingFloor::SetMoving(bool bMove)
{
    if (!HasAuthority())
        return;

    // 이미 같은 상태면 아무 것도 안 함
    if (bIsMoving == bMove)
        return;

    bIsMoving = bMove;

    if (bIsMoving)
    {
        // ✅ 재개: 지금 위치에서부터 남은 거리만큼 계속 가도록 세그먼트 갱신
        SetupSegmentFromCurrent();
    }
    else
    {
        // ✅ 정지: 그냥 멈춤. 위치/방향/세그먼트는 유지
        // (다시 켜면 SetupSegmentFromCurrent()로 남은 거리 이어서 진행)
    }
}
