#include "Gimmick/CHA/RespawnArea.h"

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ARespawnArea::ARespawnArea()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;

    AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
    SetRootComponent(AreaBox);
    AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingArrow"));
    FacingArrow->SetupAttachment(RootComponent);
}

FTransform ARespawnArea::GetSpawnTransform() const
{
    const FVector Origin = AreaBox->GetComponentLocation();
    const FVector Extent = AreaBox->GetScaledBoxExtent();
    FVector Rand = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

    const FRotator Rot = FacingArrow ? FacingArrow->GetComponentRotation() : GetActorRotation();

    if (!bProjectToGround || !GetWorld())
    {
        return FTransform(Rot, Rand);
    }

    const FVector Start = Rand + FVector(0, 0, TraceStartHeight);
    const FVector End = Start - FVector(0, 0, TraceDownDistance);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(RespawnAreaTrace), false);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params))
    {
        Rand = Hit.Location;
    }

    return FTransform(Rot, Rand);
}
