// MoveablePlatform.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveablePlatform.generated.h"

UCLASS()
class PROJECT_14_API AMoveablePlatform : public AActor
{
    GENERATED_BODY()

public:
    AMoveablePlatform();

    // 발판 메쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* PlatformMesh;

    // 시작 위치 기준으로 이동 가능한 최소 / 최대 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    FVector MinOffset = FVector(-3100.f, -840.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    FVector MaxOffset = FVector(3100.f, 840.f, 0.f);

    // 속도 (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    float MoveSpeed = 600.f;

    // 버튼이 매 프레임 호출하는 이동 함수
    void MoveStep(const FVector& Direction, float DeltaTime);

protected:
    virtual void BeginPlay() override;

private:
    FVector StartLocation;
};
