#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "FloorDoorOpen.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ARaiseDoor;          // 네 프로젝트에 RaiseDoor.h/.cpp가 있으니 이 타입 맞음
class ATimerFloorButton;   // 타이머 바닥 버튼을 쓸 경우

UCLASS()
class PROJECT_14_API AFloorDoorOpen : public AActor
{
    GENERATED_BODY()

public:
    AFloorDoorOpen();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* Trigger;

    // (선택) 이걸 누르면 타이머 바닥을 잠금(25초 캔슬)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link")
    ATimerFloorButton* FloorTimerButtonRef = nullptr;

    // (선택) 이걸 누르면 문 열기
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link")
    ARaiseDoor* TargetDoor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bOneTimeUse = true;

    bool bUsed = false;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
