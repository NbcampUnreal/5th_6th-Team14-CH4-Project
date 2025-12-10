#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingFloor.generated.h"

UCLASS()
class AMovingFloor : public AActor
{
    GENERATED_BODY()

public:
    AMovingFloor();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Floor")
    void StartMove();

    // 🔴 버튼에서 접근해야 하니까 public + BlueprintReadWrite
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
    FName FloorID = "Floor01";

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* FloorMesh;

    UPROPERTY(EditAnywhere, Category = "Movement")
    FVector TargetOffset = FVector(500.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveTime = 2.f;

private:
    FVector StartLocation;
    FVector EndLocation;

    bool  bIsMoving = false;
    float ElapsedTime = 0.f;
};