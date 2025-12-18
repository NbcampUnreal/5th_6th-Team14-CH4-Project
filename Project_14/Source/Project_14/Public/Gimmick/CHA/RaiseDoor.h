#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaiseDoor.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API ARaiseDoor : public AActor
{
    GENERATED_BODY()

public:
    ARaiseDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void OpenDoor();

    UFUNCTION(Server, Reliable)
    void ServerOpenDoor();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* DoorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpenHeight = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpenDuration = 1.0f;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Door")
    bool bIsOpen = false;

    FVector ClosedLocation;
    FVector OpenLocation;

    FTimerHandle OpenTimer;
    float Elapsed = 0.0f;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastStartOpen();

    void TickOpen();
};
