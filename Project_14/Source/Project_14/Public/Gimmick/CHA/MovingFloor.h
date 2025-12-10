// MovingFloor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingFloor.generated.h"

class UStaticMeshComponent;

UCLASS()
class AMovingFloor : public AActor
{
    GENERATED_BODY()

public:
    AMovingFloor();
    virtual void Tick(float DeltaTime) override;

    // 버튼에서 호출할 함수들
    UFUNCTION(BlueprintCallable, Category = "Floor")
    void StartMove();

    // 발판 위에 있을 때만 움직이게 ON/OFF
    UFUNCTION(BlueprintCallable, Category = "Floor")
    void SetMoving(bool bMove);

    // 버튼에서 찾을 때 사용할 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
    FName FloorID = "Floor01";

protected:
    virtual void BeginPlay() override;

    // 바닥 메시
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* FloorMesh;

    // 시작 위치에서 얼마나 이동할지 (이 값이 "이동 범위")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector TargetOffset = FVector(-2400.f, 0.f, 0.f);

    // 한 번 왼쪽→오른쪽(혹은 오른쪽→왼쪽) 가는 데 걸리는 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveTime = 7.5f;

private:
    FVector StartLocation;
    FVector EndLocation;

    bool  bIsMoving = false;  // 움직이는 중인지
    float ElapsedTime = 0.f;    // 현재 왕복 구간에서 지난 시간

    // true: Start -> End, false: End -> Start
    bool  bForward = true;
};
