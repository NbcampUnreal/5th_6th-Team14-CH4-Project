// WallToggleButton.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallToggleButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AWallToggle;

UCLASS()
class PROJECT_14_API AWallToggleButton : public AActor
{
    GENERATED_BODY()

public:
    AWallToggleButton();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
    UBoxComponent* TriggerBox;

    // 이 버튼이 ON/OFF 시킬 벽
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
    AWallToggle* TargetWall;

    // 버튼 위에 플레이어가 올라와 있는지
    bool bPlayerOnButton = false;

    // 내려간 뒤 1초 후에 벽을 끄기 위한 타이머
    FTimerHandle HideWallTimerHandle;

protected:
    virtual void BeginPlay() override;

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

    // 타이머가 호출하는 실제 벽 끄기 함수
    void HideWall();
};
