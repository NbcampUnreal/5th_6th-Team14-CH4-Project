// RandomGlassTile.cpp

#include "Gimmick/CHA/RandomGlassTile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "EngineUtils.h"      // TActorIterator
#include "Math/UnrealMathUtility.h" // FMath::RandBool

ARandomGlassTile::ARandomGlassTile()
{
    PrimaryActorTick.bCanEverTick = false;

    // 메쉬
    TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
    SetRootComponent(TileMesh);

    // 트리거 박스
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

    // =========================
    // ① Pair 모드 (쌍으로 쓰는 타일)
    // =========================
    if (PairId >= 0)
    {
        TArray<ARandomGlassTile*> PairTiles;

        // 같은 PairId 가진 타일 전부 찾기
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
            // 둘 다 BeginPlay가 돌지만, 한 Actor만 대표로 세팅 담당
            if (this == PairTiles[0])
            {
                // 🔥 아무 시드도 안 건드리고, 전역 랜덤에서 bool 하나만 뽑는다.
                const bool bFirstIsSafe = FMath::RandBool();   // true/false 50:50

                PairTiles[0]->bIsSafeTile = bFirstIsSafe;
                PairTiles[1]->bIsSafeTile = !bFirstIsSafe;

                UE_LOG(LogTemp, Warning,
                    TEXT("PairId %d : %s SAFE=%s,  %s SAFE=%s"),
                    PairId,
                    *PairTiles[0]->GetName(), PairTiles[0]->bIsSafeTile ? TEXT("true") : TEXT("false"),
                    *PairTiles[1]->GetName(), PairTiles[1]->bIsSafeTile ? TEXT("true") : TEXT("false"));
            }
        }
        else if (PairTiles.Num() > 0)
        {
            UE_LOG(LogTemp, Error,
                TEXT("RandomGlassTile PairId %d 에 타일이 %d개 있습니다. (쌍당 2개만 사용해야 합니다)"),
                PairId, PairTiles.Num());
        }
    }
    // =========================
    // ② 단독 타일 모드 (PairId < 0)
    // =========================
    else if (bRandomizeAtBeginPlay)
    {
        bIsSafeTile = FMath::RandBool();

        UE_LOG(LogTemp, Warning,
            TEXT("RandomGlassTile %s (single) : SAFE=%s"),
            *GetName(),
            bIsSafeTile ? TEXT("true") : TEXT("false"));
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

    // 충돌 끄고
    TileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 물리 켜서 아래로 떨어지게
    TileMesh->SetSimulatePhysics(true);

    // 일정 시간 뒤 액터 삭제
    if (LifeTimeAfterBreak > 0.f)
    {
        SetLifeSpan(LifeTimeAfterBreak);
    }
}
