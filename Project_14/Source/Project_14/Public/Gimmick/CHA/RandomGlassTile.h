#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomGlassTile.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UPrimitiveComponent;

UCLASS()
class PROJECT_14_API ARandomGlassTile : public AActor
{
    GENERATED_BODY()

public:
    ARandomGlassTile();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* TileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    bool bRandomizeAtBeginPlay = true;

    // ✅ 서버가 결정한 안전/위험을 클라에 동기화
    UPROPERTY(ReplicatedUsing = OnRep_IsSafeTile, EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    bool bIsSafeTile = true;

    UFUNCTION()
    void OnRep_IsSafeTile() {}

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    int32 PairId = -1;

    bool bAlreadyTriggered = false;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
