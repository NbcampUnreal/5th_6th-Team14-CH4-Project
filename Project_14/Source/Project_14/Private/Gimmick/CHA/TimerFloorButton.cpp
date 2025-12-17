#include "Gimmick/CHA/TimerFloorButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

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

    // ✅ 오버랩은 서버에서만 처리(멀티 중복/오작동 방지)
    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATimerFloorButton::OnBeginOverlap);
    }
}

void ATimerFloorButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATimerFloorButton, bLocked);
    DOREPLIFETIME(ATimerFloorButton, bPressed);
}

void ATimerFloorButton::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor) return;

    if (!Cast<APawn>(OtherActor)) return;

    PressButton(); // 서버에서만 실행됨
}

void ATimerFloorButton::PressButton()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;
    if (bLocked) return;
    if (bPressed) return; // 한 번만 시작(원하면 제거)

    bPressed = true;

    // ✅ 모든 클라에 바닥 ON 반영
    MulticastSetFloorActive(true);

    // ✅ 25초 후 OFF는 서버 타이머로
    GetWorldTimerManager().ClearTimer(DisappearTimer);
    GetWorldTimerManager().SetTimer(DisappearTimer, this, &ATimerFloorButton::HideFloor_Server, DisappearDelay, false);
}

void ATimerFloorButton::LockFloor()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;

    bLocked = true;
    GetWorldTimerManager().ClearTimer(DisappearTimer);

    // ✅ 잠금 시 바닥 확정 ON
    MulticastSetFloorActive(true);
}

void ATimerFloorButton::HideFloor_Server()
{
    if (!HasAuthority()) return;
    if (!TargetFloor) return;
    if (bLocked) return;

    // ✅ 모든 클라에 바닥 OFF 반영
    MulticastSetFloorActive(false);
}

void ATimerFloorButton::MulticastSetFloorActive_Implementation(bool bActive)
{
    if (!TargetFloor) return;

    TargetFloor->SetActorHiddenInGame(!bActive);
    TargetFloor->SetActorEnableCollision(bActive);
}
