#include "Gimmick/CHA/TimerFloorButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

#include "Gimmick/CHA/TreapTile.h"   // ✅ TreapTile 연결

ATimerFloorButton::ATimerFloorButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(false);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->SetupAttachment(RootComponent);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetGenerateOverlapEvents(true);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATimerFloorButton::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 오버랩은 서버에서만 처리
    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATimerFloorButton::OnBeginOverlap);

        // 시작은 OFF (원하는 시작 상태가 있으면 ResetPuzzle로 제어)
        MulticastSetFloorActive(false);
        ApplyTreapTiles_Server(false);
    }
}

void ATimerFloorButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATimerFloorButton, TargetFloor);
    DOREPLIFETIME(ATimerFloorButton, bLocked);
    DOREPLIFETIME(ATimerFloorButton, bPressed);
}

void ATimerFloorButton::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor) return;
    if (!Cast<APawn>(OtherActor)) return;

    PressButton();
}

void ATimerFloorButton::ApplyTreapTiles_Server(bool bActive)
{
    if (!HasAuthority()) return;

    for (ATreapTile* Tile : TreapTiles)
    {
        if (IsValid(Tile))
        {
            Tile->ServerSetSystemActive(bActive);
        }
    }
}

void ATimerFloorButton::PressButton()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;
    if (bLocked) return;
    if (bPressed) return;

    bPressed = true;

    // ✅ 바닥 ON + 방해벽 시스템 ON
    MulticastSetFloorActive(true);
    ApplyTreapTiles_Server(true);

    // ✅ 25초 후 OFF
    GetWorldTimerManager().ClearTimer(DisappearTimer);
    GetWorldTimerManager().SetTimer(DisappearTimer, this, &ATimerFloorButton::HideFloor_Server, DisappearDelay, false);
}

void ATimerFloorButton::LockFloor()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;

    bLocked = true;

    GetWorldTimerManager().ClearTimer(DisappearTimer);

    // 바닥 확정 ON (방해벽은 계속 ON으로 유지 중)
    MulticastSetFloorActive(true);
    ApplyTreapTiles_Server(true);
}

void ATimerFloorButton::StopGimmick(bool bKeepFloorOn)
{
    if (!HasAuthority()) return;

    // ✅ 성공 처리: 타이머 종료 + 다시 안 꺼지게 잠금
    bLocked = true;
    bPressed = false;

    GetWorldTimerManager().ClearTimer(DisappearTimer);

    // ✅ 방해벽 시스템 OFF
    ApplyTreapTiles_Server(false);

    // ✅ 바닥은 유지(true) / 끄고 싶으면 false
    MulticastSetFloorActive(bKeepFloorOn);
}

void ATimerFloorButton::HideFloor_Server()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;
    if (bLocked) return;

    // ✅ 바닥 OFF + 방해벽 OFF
    MulticastSetFloorActive(false);
    ApplyTreapTiles_Server(false);

    // ✅ 25초 끝나면 다시 누를 수 있게
    bPressed = false;
}

void ATimerFloorButton::ResetPuzzle(bool bStartWithFloorOn)
{
    if (!HasAuthority()) return;

    GetWorldTimerManager().ClearTimer(DisappearTimer);
    bLocked = false;
    bPressed = false;

    MulticastSetFloorActive(bStartWithFloorOn);
    ApplyTreapTiles_Server(bStartWithFloorOn);

    if (bStartWithFloorOn)
    {
        bPressed = true;
        GetWorldTimerManager().SetTimer(DisappearTimer, this, &ATimerFloorButton::HideFloor_Server, DisappearDelay, false);
    }
}

void ATimerFloorButton::MulticastSetFloorActive_Implementation(bool bActive)
{
    if (!TargetFloor) return;

    TargetFloor->SetActorHiddenInGame(!bActive);
    TargetFloor->SetActorEnableCollision(bActive);
}
