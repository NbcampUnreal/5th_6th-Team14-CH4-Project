#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingFloor.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AMovingFloor : public AActor
{
    GENERATED_BODY()

public:
    AMovingFloor();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Floor")
    void StartMove();

    UFUNCTION(BlueprintCallable, Category = "Floor")
    void SetMoving(bool bMove);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
    FName FloorID = "Floor01";

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* FloorMesh;

    // A에서 B로 이동할 때의 목표 오프셋(고정 끝점 B = A + TargetOffset)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector TargetOffset = FVector(0.f, -2700.f, 0.f);

    // A <-> B 전체 길이를 이동하는 데 걸리는 시간(속도 기준)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.01"))
    float MoveTime = 7.5f;

private:
    // 고정된 두 끝점
    FVector PointA;
    FVector PointB;

    // 현재 이동 세그먼트(일시정지/재개 시 갱신)
    FVector SegmentStart;
    FVector SegmentEnd;

    bool  bIsMoving = false;
    bool  bForward = true;     // true면 B로, false면 A로
    float ElapsedTime = 0.f;
    float SegmentDuration = 0.f; // 현재 세그먼트의 남은 이동 시간

private:
    void SetupSegmentFromCurrent(); // 현재 위치 기준으로 남은 거리만큼 세그먼트/시간 계산
};
