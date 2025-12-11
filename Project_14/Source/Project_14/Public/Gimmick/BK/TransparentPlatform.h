#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TransparentPlatform.generated.h"

UCLASS()
class PROJECT_14_API ATransparentPlatform : public AActor
{
    GENERATED_BODY()

public:
    ATransparentPlatform();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* PlatformMesh;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    float VisibleTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    float InvisibleTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Platform|Visibility")
    bool bLoop = true;

    float Timer = 0.0f;
    bool bIsVisible = true;
};