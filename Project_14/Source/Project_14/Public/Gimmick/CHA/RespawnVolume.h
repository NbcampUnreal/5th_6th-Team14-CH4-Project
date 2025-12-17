#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnVolume.generated.h"

class UBoxComponent;
class ARespawnArea;

UCLASS()
class PROJECT_14_API ARespawnVolume : public AActor
{
    GENERATED_BODY()

public:
    ARespawnVolume();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Respawn")
    UBoxComponent* Box;

    // 에디터에서 RespawnArea 액터를 지정
    UPROPERTY(EditInstanceOnly, Category = "Respawn")
    ARespawnArea* RespawnArea;

    UPROPERTY(EditAnywhere, Category = "Respawn")
    float CooldownSeconds = 1.0f;

    TMap<TWeakObjectPtr<AActor>, float> LastRespawnTime;

    UFUNCTION()
    void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void TeleportRespawn(AActor* Victim);
};
