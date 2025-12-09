#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Button_CHA.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AMovingFloor;

UCLASS()
class AButton_CHA : public AActor
{
    GENERATED_BODY()

public:
    AButton_CHA();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerBox;

    // 🔹 내가 조종할 바닥의 ID (FloorID랑 같은 값으로 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    FName TargetFloorID = "Floor01";

private:
    // 🔹 실제로 찾은 바닥(코드에서만 사용)
    AMovingFloor* TargetFloor = nullptr;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    bool bAlreadyActivated = false;
};
