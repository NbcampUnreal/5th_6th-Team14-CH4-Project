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

    // ���� ���� ��ġ�� �������� ����/�� ��ġ ���
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

    // ���� ����(0 ~ 1)
    ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(ElapsedTime / MoveTime, 0.f, 1.f);

    // ���⿡ ���� ����/�� ��ġ�� �ٲ㼭 Lerp
    const FVector From = bForward ? StartLocation : EndLocation;
    const FVector To = bForward ? EndLocation : StartLocation;

    const FVector NewLocation = FMath::Lerp(From, To, Alpha);
    SetActorLocation(NewLocation);

    UE_LOG(LogTemp, Verbose, TEXT("[MovingFloor] Tick: %s, Alpha=%.2f, NewLocation=%s"),
        *GetName(), Alpha, *NewLocation.ToString());

    // ���� ������ �����ϸ� ���� ��ȯ + �ð� ���� �� ��� �պ�
    if (Alpha >= 1.f)
    {
        bForward = !bForward;
        ElapsedTime = 0.f;

        UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] ���� ��ȯ: %s, bForward=%s"),
            *GetName(), bForward ? TEXT("true") : TEXT("false"));
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
    bForward = true;   // ó���� �׻� Start -> End �������
    ElapsedTime = 0.f;
}

void AMovingFloor::SetMoving(bool bMove)
{
    bIsMoving = bMove;

    UE_LOG(LogTemp, Warning, TEXT("[MovingFloor] SetMoving(%s): %s"),
        bMove ? TEXT("true") : TEXT("false"),
        *GetName());
}
