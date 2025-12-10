// RandomGlassTile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomGlassTile.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Squid Game style glass tile.
 * - If PairId >= 0 : exactly one tile in the pair is safe, the other breaks.
 * - If PairId  < 0 : works as a single tile with 50% chance.
 */
UCLASS()
class PROJECT_14_API ARandomGlassTile : public AActor
{
    GENERATED_BODY()

public:
    ARandomGlassTile();

protected:
    virtual void BeginPlay() override;

    /** Visual mesh of the tile */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* TileMesh;

    /** Trigger to detect when the player steps on the tile */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* TriggerBox;

    /** If PairId < 0, decide safe/break randomly at BeginPlay */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    bool bRandomizeAtBeginPlay = true;

    /** Is this tile safe? (true = safe, false = will break) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    bool bIsSafeTile = true;

    /** Delay before the tile actually breaks (seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    float BreakDelay = 0.3f;

    /** Life time after breaking (seconds). 0 = never destroy actor automatically. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    float LifeTimeAfterBreak = 3.0f;

    /**
     * If PairId >= 0:
     *   Tiles with the same PairId are treated as a pair (2 tiles).
     *   Exactly one of them becomes safe, the other becomes breakable.
     * If PairId < 0:
     *   This tile works as a single independent tile.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GlassTrap")
    int32 PairId = -1;

    /** Make sure the tile only triggers once. */
    bool bAlreadyTriggered = false;

    FTimerHandle BreakTimerHandle;

    /** Called when the player steps on the tile. */
    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    /** Actual breaking logic. */
    UFUNCTION()
    void BreakTile();

public:
    virtual void Tick(float DeltaTime) override;
};
