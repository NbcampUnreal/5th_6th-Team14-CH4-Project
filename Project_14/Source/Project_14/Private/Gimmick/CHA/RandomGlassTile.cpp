#include "Gimmick/CHA/RandomGlassTile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"

ARandomGlassTile::ARandomGlassTile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    SetRootComponent(TileMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->InitBoxExtent(FVector(50.f, 50.f, 30.f));

    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARandomGlassTile::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 서버만 랜덤 결정 + 오버랩 처리
    if (!HasAuthority())
        return;

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARandomGlassTile::OnOverlapBegin);

    if (PairId >= 0)
    {
        TArray<ARandomGlassTile*> PairTiles;

        for (TActorIterator<ARandomGlassTile> It(GetWorld()); It; ++It)
        {
            ARandomGlassTile* Tile = *It;
            if (Tile && Tile->PairId == PairId)
            {
                PairTiles.Add(Tile);
            }
        }

        if (PairTiles.Num() == 2)
        {
            if (this == PairTiles[0])
            {
                const bool bFirstIsSafe = FMath::RandBool();
                PairTiles[0]->bIsSafeTile = bFirstIsSafe;
                PairTiles[1]->bIsSafeTile = !bFirstIsSafe;
            }
        }
    }
    else if (bRandomizeAtBeginPlay)
    {
        bIsSafeTile = FMath::RandBool();
    }
}

void ARandomGlassTile::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    if (bAlreadyTriggered || !OtherActor)
        return;

    if (!Cast<ACharacter>(OtherActor))
        return;

    bAlreadyTriggered = true;

    if (bIsSafeTile)
        return;

    // ✅ 위험 타일이면 서버에서 즉시 파괴(클라에도 자동 전파)
    Destroy();
}

void ARandomGlassTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARandomGlassTile, bIsSafeTile);
}
