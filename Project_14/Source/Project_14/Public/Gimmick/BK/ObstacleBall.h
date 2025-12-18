#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleBall.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AObstacleBall : public AActor
{
    GENERATED_BODY()

public:
    AObstacleBall();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void EnablePhysics();

    UFUNCTION()
    void OnHit(
        UPrimitiveComponent* HitComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float NormalMass = 10000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float WeakJumpBounceStrength = 900.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float StrongPushBounceStrength = 250.f;
};
