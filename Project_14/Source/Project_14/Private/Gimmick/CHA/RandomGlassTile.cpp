// RandomGlassTile.cpp

#include "Gimmick/CHA/RandomGlassTile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "EngineUtils.h"      // TActorIterator
#include "Math/RandomStream.h"
#include "Misc/DateTime.h"

ARandomGlassTile::ARandomGlassTile()
{
    PrimaryActorTick.bCanEverTick = false;

    // Mesh
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    SetRootComponent(TileMesh);

    // Trigger
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

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARandomGlassTile::OnOverlapBegin);

    // Pair 모드: 같은 PairId 가진 타일들을 월드에서 찾아서 한 쌍으로 처리
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
            // 둘 다 BeginPlay가 돌지만, 딱 하나만 세팅하도록 "대표"를 정함
            if (this == PairTiles[0])
            {
                // 🔥 실행 시간 + PairId를 섞어서 매번 다른 시드 생성
                int64 NowTicks = FDateTime::Now().GetTicks();
                int32 Seed = static_cast<int32>(NowTicks & 0xFFFFFFFF) ^ PairId;

                FRandomStream RandStream;
                RandStream.Initialize(Seed);

                bool bFirstSafe = (RandStream.FRand() < 0.5f);

                PairTiles[0]->bIsSafeTile = bFirstSafe;
                PairTiles[1]->bIsSafeTile = !bFirstSafe;

                UE_LOG(LogTemp, Warning,
                    TEXT("PairId %d set: %s = %s, %s = %s (Seed=%d)"),
                    PairId,
                    *PairTiles[0]->GetName(),
                    PairTiles[0]->bIsSafeTile ? TEXT("SAFE") : TEXT("BREAK"),
                    *PairTiles[1]->GetName(),
                    PairTiles[1]->bIsSafeTile ? TEXT("SAFE") : TEXT("BREAK"),
                    Seed);
            }
        }
        else if (PairTiles.Num() > 2)
        {
            UE_LOG(LogTemp, Error,
                TEXT("RandomGlassTile PairId %d has %d tiles. Only 2 tiles are allowed per PairId."),
                PairId, PairTiles.Num());
        }
        // Num() == 1 인 경우: 아직 쌍이 완성 안 된 상태라 여기서는 아무 것도 안 함
    }
    // Single independent tile (PairId < 0 일 때만)
    else if (bRandomizeAtBeginPlay)
    {
        int64 NowTicks = FDateTime::Now().GetTicks();
        int32 Seed = static_cast<int32>(NowTicks & 0xFFFFFFFF);

        FRandomStream RandStream;
        RandStream.Initialize(Seed);

        bIsSafeTile = (RandStream.FRand() < 0.5f);

        UE_LOG(LogTemp, Warning,
            TEXT("RandomGlassTile %s (single) : bIsSafeTile = %s (Seed=%d)"),
            *GetName(),
            bIsSafeTile ? TEXT("SAFE") : TEXT("BREAK"),
            Seed);
    }
}

void ARandomGlassTile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARandomGlassTile::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (bAlreadyTriggered || !OtherActor)
    {
        return;
    }

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }

    bAlreadyTriggered = true;

    if (bIsSafeTile)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("RandomGlassTile %s : SAFE tile, no break."),
            *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("RandomGlassTile %s : DANGER tile, will break."),
            *GetName());

        GetWorldTimerManager().SetTimer(
            BreakTimerHandle,
            this,
            &ARandomGlassTile::BreakTile,
            BreakDelay,
            false);
    }
}

void ARandomGlassTile::BreakTile()
{
    UE_LOG(LogTemp, Warning,
        TEXT("RandomGlassTile %s : BREAK!"),
        *GetName());

    // Disable collision so the player falls
    TileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Let the mesh fall down with physics
    TileMesh->SetSimulatePhysics(true);

    if (LifeTimeAfterBreak > 0.f)
    {
        SetLifeSpan(LifeTimeAfterBreak);
    }
}
