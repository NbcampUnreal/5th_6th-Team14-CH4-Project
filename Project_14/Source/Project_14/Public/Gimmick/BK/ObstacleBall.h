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

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;
};
