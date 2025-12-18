#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"   // FHitResult
#include "TimerManager.h"         // FTimerHandle
#include "TimerFloorButton.generated.h"  // ✅ 반드시 include 중 마지막!

class UStaticMeshComponent;
class UBoxComponent;
class UPrimitiveComponent;
class ATreapTile;

UCLASS()
class PROJECT_14_API ATimerFloorButton : public AActor
{
    GENERATED_BODY()

public:
    ATimerFloorButton();

    // 버튼(밟기)로 25초 타이머 시작
    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void PressButton();

    // 잠금(25초 타이머 취소 + 바닥 유지)
    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void LockFloor();

    // 재시작(리셋)
    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void ResetPuzzle(bool bStartWithFloorOn = false);

    // ✅ 문 열림 성공 버튼에서 호출:
    // - 25초 타이머 즉시 종료
    // - Treap(방해벽) OFF
    // - 바닥은 유지(true)
    UFUNCTION(BlueprintCallable, Category = "TimerFloor")
    void StopGimmick(bool bKeepFloorOn = true);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* Trigger;

    // 레벨에 배치한 바닥 액터 지정
    UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadWrite, Category = "TimerFloor")
    AActor* TargetFloor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimerFloor")
    float DisappearDelay = 25.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "TimerFloor")
    bool bLocked = false;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "TimerFloor")
    bool bPressed = false;

    // ✅ 25초 시스템에 같이 묶일 TreapTile(방해벽)들
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "TimerFloor")
    TArray<ATreapTile*> TreapTiles;

    FTimerHandle DisappearTimer;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void HideFloor_Server();
    void ApplyTreapTiles_Server(bool bActive);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastSetFloorActive(bool bActive);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
