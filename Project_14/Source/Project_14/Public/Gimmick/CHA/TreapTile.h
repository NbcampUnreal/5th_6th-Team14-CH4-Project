#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreapTile.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class PROJECT_14_API ATreapTile : public AActor
{
    GENERATED_BODY()

public:
    ATreapTile();

    // (선택) TimerFloorButton 같은 다른 시스템에서 ON/OFF 시키고 싶을 때 사용
    UFUNCTION(BlueprintCallable, Category="Treap|System")
    void ServerSetSystemActive(bool bActive);

protected:
    virtual void BeginPlay() override;

    // 범위 감지 트리거
    UPROPERTY(VisibleAnywhere, Category="Components")
    UBoxComponent* Trigger;

    // 실제로 생겼다/사라지는 벽(메쉬)
    UPROPERTY(VisibleAnywhere, Category="Components")
    UStaticMeshComponent* WallMesh;

    // 시스템 자체 ON/OFF (기본 true라서 단독 사용 가능)
    UPROPERTY(ReplicatedUsing=OnRep_SystemActive, VisibleAnywhere, BlueprintReadOnly, Category="Treap|System")
    bool bSystemActive = true;

    // 벽이 현재 활성화(보임/막힘) 상태인지
    UPROPERTY(ReplicatedUsing=OnRep_WallActive, VisibleAnywhere, BlueprintReadOnly, Category="Treap|Wall")
    bool bWallActive = false;

    // 벽을 “앞쪽”에 두고 싶으면 BP에서 조절(로컬 오프셋)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Treap|Wall")
    FVector WallLocalOffset = FVector(200.f, 0.f, 0.f);

    // 벽 크기(스케일)도 BP에서 조절 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Treap|Wall")
    FVector WallLocalScale = FVector(1.0f, 3.0f, 3.0f);

    // 범위에 들어오면 벽 ON, 나오면 벽 OFF
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Treap|Wall")
    bool bUseEndOverlapToDisable = true;

    // 들어온 뒤 자동으로 벽을 내리고 싶으면 (0이면 자동OFF 안함)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Treap|Wall")
    float AutoDisableDelay = 0.0f;

    FTimerHandle AutoDisableTimer;

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void SetWallActive_Server(bool bActive);
    void ApplyWallVisual(bool bActive);

    UFUNCTION()
    void OnRep_WallActive();

    UFUNCTION()
    void OnRep_SystemActive();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
