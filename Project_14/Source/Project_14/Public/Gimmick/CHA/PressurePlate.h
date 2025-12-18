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

    // ✅ 레벨(인스턴스)에서 버튼 액터를 직접 찍을 수 있게 하는 핵심 설정
    //    - 타입: ATimerFloorButton*  (클래스가 아니라 "월드에 있는 액터 참조")
    //    - EditInstanceOnly: 레벨에 배치한 후 디테일에서 선택 가능
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Link", meta = (DisplayName = "Button Ref"))
    ATimerFloorButton* ButtonRef = nullptr;

    // 큐브/바위에 Rock 태그 달기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule")
    FName RequiredTag = "Rock";

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
