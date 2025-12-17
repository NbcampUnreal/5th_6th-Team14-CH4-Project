#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerFloorButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class PROJECT_14_API ATimerFloorButton : public AActor
{
    GENERATED_BODY()

public:
    ATimerFloorButton();

    // (참고) BP에서 직접 호출하고 싶으면 써도 되지만,
    // 오버랩 기반이면 서버에서 자동으로 처리됨
    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void PressButton();

    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void LockFloor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* Trigger;

    // 레벨에 배치한 바닥 액터 지정(StaticMeshActor 등)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "TimerFloor")
    AActor* TargetFloor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimerFloor")
    float DisappearDelay = 25.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "TimerFloor")
    bool bLocked = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "TimerFloor")
    bool bPressed = false;

    FTimerHandle DisappearTimer;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void HideFloor_Server();

    // ✅ 모든 클라(서버 포함)에게 바닥 상태 반영
    UFUNCTION(NetMulticast, Reliable)
    void MulticastSetFloorActive(bool bActive);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
