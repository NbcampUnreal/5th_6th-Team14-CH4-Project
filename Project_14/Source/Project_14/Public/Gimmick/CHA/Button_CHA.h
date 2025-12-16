#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Button_CHA.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AMovingFloor;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API AButton_CHA : public AActor
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

    // 버튼이 제어할 MovingFloor의 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    FName TargetFloorID = "Floor01";

private:
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
};
