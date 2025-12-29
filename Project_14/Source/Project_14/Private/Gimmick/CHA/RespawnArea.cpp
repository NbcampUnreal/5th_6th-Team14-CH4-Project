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
    if (!AreaBox)
    {
        return GetActorTransform();
    }

    const FVector Origin = AreaBox->GetComponentLocation();
    const FVector Extent = AreaBox->GetScaledBoxExtent();

    // ✅ XY만 랜덤으로 뽑고, Z는 Origin.Z로 고정 (Z 랜덤으로 지붕쪽 뽑히는 문제 방지)
    FVector Rand = UKismetMathLibrary::RandomPointInBoundingBox(
        Origin,
        FVector(Extent.X, Extent.Y, 0.f)
    );
    Rand.Z = Origin.Z;

    const FRotator Rot = (FacingArrow ? FacingArrow->GetComponentRotation() : GetActorRotation());

    if (!bProjectToGround || !GetWorld())
    {
        return FTransform(Rot, Rand);
    }

    // ✅ 트레이스 시작은 "박스 상단"에서, 끝은 "박스 하단 아래"까지
    const float BoxTopZ = Origin.Z + Extent.Z;
    const float BoxBottomZ = Origin.Z - Extent.Z;

    const FVector Start(Rand.X, Rand.Y, BoxTopZ + 50.f);                 // 박스 상단에서 살짝 위
    const FVector End(Rand.X, Rand.Y, BoxBottomZ - TraceDownDistance); // 박스 하단 아래로 충분히

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(RespawnAreaTrace), false);
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params))
    {
        Rand = Hit.Location;
    }

    return FTransform(Rot, Rand);
}
