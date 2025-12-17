#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ATimerFloorButton;

UCLASS()
class PROJECT_14_API APressurePlate : public AActor
{
    GENERATED_BODY()

public:
    APressurePlate();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* PlateMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* Trigger;

    // 레벨에서 버튼 액터 지정(서버가 이걸로 LockFloor 호출)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link")
    ATimerFloorButton* ButtonRef = nullptr;

    // 큐브/바위에 Rock 태그 달기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule")
    FName RequiredTag = "Rock";

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
