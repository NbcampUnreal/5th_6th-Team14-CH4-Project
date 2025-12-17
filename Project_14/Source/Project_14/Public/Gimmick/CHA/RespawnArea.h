#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnArea.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class PROJECT_14_API ARespawnArea : public AActor
{
    GENERATED_BODY()

public:
    ARespawnArea();

    UFUNCTION(BlueprintCallable, Category = "Respawn")
    FTransform GetSpawnTransform() const;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* AreaBox;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UArrowComponent* FacingArrow;

    UPROPERTY(EditAnywhere, Category = "Respawn")
    bool bProjectToGround = true;

    UPROPERTY(EditAnywhere, Category = "Respawn", meta = (EditCondition = "bProjectToGround"))
    float TraceStartHeight = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Respawn", meta = (EditCondition = "bProjectToGround"))
    float TraceDownDistance = 5000.f;

    UPROPERTY(EditAnywhere, Category = "Respawn", meta = (EditCondition = "bProjectToGround"))
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
};
