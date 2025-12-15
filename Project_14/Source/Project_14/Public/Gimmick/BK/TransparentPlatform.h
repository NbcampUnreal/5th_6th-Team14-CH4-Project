#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TransparentPlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API ATransparentPlatform : public AActor
{
    GENERATED_BODY()

public:
    ATransparentPlatform();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PlatformMesh;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    float VisibleTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    float InvisibleTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    bool bLoop = true;

    UPROPERTY(ReplicatedUsing = OnRep_VisibilityState)
    bool bIsVisible = true;

    float Timer = 0.f;

    UFUNCTION()
    void OnRep_VisibilityState();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
