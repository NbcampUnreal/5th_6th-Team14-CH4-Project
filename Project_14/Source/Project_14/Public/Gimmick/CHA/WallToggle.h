#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallToggle.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AWallToggle : public AActor
{
    GENERATED_BODY()

public:
    AWallToggle();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
    UStaticMeshComponent* WallMesh;

    // ✅ 상태를 네트워크로 동기화
    UPROPERTY(ReplicatedUsing = OnRep_WallEnabled)
    bool bWallEnabled = false;

    UFUNCTION()
    void OnRep_WallEnabled();

    void ApplyWallEnabled(bool bEnable);

public:
    UFUNCTION(BlueprintCallable, Category = "Wall")
    void SetWallEnabled(bool bEnable);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
