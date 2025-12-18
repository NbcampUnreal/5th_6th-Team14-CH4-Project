#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingFloor.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AMovingFloor : public AActor
{
    GENERATED_BODY()

public:
    AMovingFloor();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Floor")
    void StartMove();

    UFUNCTION(BlueprintCallable, Category = "Floor")
    void SetMoving(bool bMove);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
    FName FloorID = "Floor01";

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* FloorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector TargetOffset = FVector(0.f, -2400.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveTime = 7.5f;

private:
    FVector StartLocation;
    FVector EndLocation;

    bool  bIsMoving = false;
    float ElapsedTime = 0.f;
    bool  bForward = true;
};
