#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallToggleButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AWallToggle;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API AWallToggleButton : public AActor
{
    GENERATED_BODY()

public:
    AWallToggleButton();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button")
    UBoxComponent* TriggerBox;

    // ✅ 인스턴스에서 지정 권장
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Button")
    AWallToggle* TargetWall = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
    float HideDelay = 2.0f;

    bool bPlayerOnButton = false;
    FTimerHandle HideWallTimerHandle;

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

    UFUNCTION()
    void HideWall();
};
