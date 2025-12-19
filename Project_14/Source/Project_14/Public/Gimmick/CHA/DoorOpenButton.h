#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorOpenButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API ADoorOpenButton : public AActor
{
    GENERATED_BODY()

public:
    ADoorOpenButton();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> ButtonMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UBoxComponent> TriggerBox;

    // ✅ 삭제할 큐브 2개 슬롯
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Button")
    TObjectPtr<AActor> TargetCube1;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Button")
    TObjectPtr<AActor> TargetCube2;

    // ✅ 한 번만 작동할지
    UPROPERTY(EditAnywhere, Category = "Button")
    bool bPressOnce = true;

    bool bPressed = false;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    void PressOnServer(AActor* PressingActor);
};
