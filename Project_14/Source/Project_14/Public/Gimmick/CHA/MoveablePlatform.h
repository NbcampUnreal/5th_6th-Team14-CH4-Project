#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveablePlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AMoveablePlatform : public AActor
{
    GENERATED_BODY()

public:
    AMoveablePlatform();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* PlatformMesh;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Move|Bounds")
    AActor* BoundsActor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move|Bounds")
    float BoundsPadding = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    FVector MinOffset = FVector(-1040.f, -100.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    FVector MaxOffset = FVector(1040.f, 3600.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
    float MoveSpeed = 600.f;

    void MoveStep(const FVector& Direction, float DeltaTime);

protected:
    virtual void BeginPlay() override;

private:
    FVector StartLocation;

    bool bUseWorldBounds = false;
    FVector WorldMin;
    FVector WorldMax;
};
