// WallToggle.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallToggle.generated.h"

UCLASS()
class PROJECT_14_API AWallToggle : public AActor
{
    GENERATED_BODY()

public:
    AWallToggle();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
    UStaticMeshComponent* WallMesh;

public:
    // true  : 벽 보이기 + 충돌 켜기
    // false : 벽 숨기기 + 충돌 끄기
    UFUNCTION(BlueprintCallable, Category = "Wall")
    void SetWallEnabled(bool bEnable);
};
