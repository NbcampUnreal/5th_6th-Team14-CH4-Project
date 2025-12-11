// MovePlatformButton.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovePlatformButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AMoveablePlatform;

UCLASS()
class PROJECT_14_API AMovePlatformButton : public AActor
{
    GENERATED_BODY()

public:
    AMovePlatformButton();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 버튼 메쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* ButtonMesh;

    // 플레이어 감지용 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* TriggerBox;

    // 움직일 발판 BP 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
    AMoveablePlatform* TargetPlatform;

    // 이동 방향 (앞으로만 쓰려면 (1,0,0) 고정)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
    FVector MoveDirection = FVector(1.f, 0.f, 0.f);

    // 버튼 위에 플레이어가 올라와 있는지
    bool bPressed = false;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);
};
