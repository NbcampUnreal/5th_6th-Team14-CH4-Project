#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovePlatformButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AMoveablePlatform;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API AMovePlatformButton : public AActor
{
    GENERATED_BODY()

public:
    AMovePlatformButton();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* TriggerBox;

    // ✅ 인스턴스에서 지정 권장
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Button")
    AMoveablePlatform* TargetPlatform = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
    FVector MoveDirection = FVector(1.f, 0.f, 0.f);

    bool bPressed = false;

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
};
