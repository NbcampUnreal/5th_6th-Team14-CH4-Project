// DoorOpenButton.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorOpenButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * 플레이어가 버튼 위에 올라오면
 * TargetWalls 배열에 들어있는 액터들을 전부 없애는 버튼
 */
UCLASS()
class PROJECT_14_API ADoorOpenButton : public AActor
{
    GENERATED_BODY()

public:
    ADoorOpenButton();

protected:
    virtual void BeginPlay() override;

    /** 버튼 모양 메쉬 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ButtonMesh;

    /** 플레이어가 밟는 트리거 박스 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* TriggerBox;

    /** 사라지게 만들 벽/문 액터들 (여러 개 가능) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    TArray<AActor*> TargetWalls;

    /** 한 번만 작동하도록 하는 플래그 */
    bool bAlreadyActivated = false;

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

public:
    virtual void Tick(float DeltaTime) override;
};
