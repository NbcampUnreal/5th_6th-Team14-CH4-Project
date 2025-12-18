#include "Gimmick/CHA/TreapTile.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ATreapTile::ATreapTile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(false);

    // Root는 Trigger로 두면 편함(범위 액터 느낌)
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);

    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetGenerateOverlapEvents(true);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    WallMesh->SetupAttachment(RootComponent);

    // 벽은 기본 숨김 + 콜리전 OFF로 시작
    WallMesh->SetHiddenInGame(true);
    WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 벽이 "진짜로 막는 용도"라면 Pawn을 Block해야 함
    WallMesh->SetCollisionResponseToAllChannels(ECR_Block);
    WallMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    WallMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ATreapTile::BeginPlay()
{
    Super::BeginPlay();

    // 벽 위치/크기 적용(에디터에서 조절한 값)
    WallMesh->SetRelativeLocation(WallLocalOffset);
    WallMesh->SetRelativeScale3D(WallLocalScale);

    // 오버랩은 서버에서만 처리
    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATreapTile::OnTriggerBeginOverlap);

        if (bUseEndOverlapToDisable)
        {
            Trigger->OnComponentEndOverlap.AddDynamic(this, &ATreapTile::OnTriggerEndOverlap);
        }
    }

    // 시작 상태 반영
    OnRep_SystemActive();
    OnRep_WallActive();
}

void ATreapTile::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (!bSystemActive) return;
    if (!OtherActor) return;

    // 캐릭터만 반응(원하면 APawn으로 바꿔도 됨)
    if (!OtherActor->IsA<ACharacter>()) return;

    // 들어오면 벽 ON
    SetWallActive_Server(true);

    // 자동 OFF 타이머(옵션)
    if (AutoDisableDelay > 0.0f)
    {
        GetWorldTimerManager().ClearTimer(AutoDisableTimer);
        GetWorldTimerManager().SetTimer(AutoDisableTimer, [this]()
            {
                if (HasAuthority())
                {
                    SetWallActive_Server(false);
                }
            }, AutoDisableDelay, false);
    }
}

void ATreapTile::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority()) return;
    if (!bSystemActive) return;
    if (!OtherActor) return;

    if (!OtherActor->IsA<ACharacter>()) return;

    // 나가면 벽 OFF(옵션)
    if (bUseEndOverlapToDisable)
    {
        GetWorldTimerManager().ClearTimer(AutoDisableTimer);
        SetWallActive_Server(false);
    }
}

void ATreapTile::SetWallActive_Server(bool bActive)
{
    if (!HasAuthority()) return;

    bWallActive = bActive;

    // 서버도 즉시 적용
    ApplyWallVisual(bWallActive);
}

void ATreapTile::ApplyWallVisual(bool bActive)
{
    // 시스템 꺼져있으면 무조건 OFF 강제
    if (!bSystemActive)
    {
        WallMesh->SetHiddenInGame(true);
        WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        return;
    }

    WallMesh->SetHiddenInGame(!bActive);
    WallMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void ATreapTile::OnRep_WallActive()
{
    ApplyWallVisual(bWallActive);
}

void ATreapTile::OnRep_SystemActive()
{
    // 시스템 OFF면 벽 강제 OFF
    if (!bSystemActive)
    {
        bWallActive = false;
    }

    ApplyWallVisual(bWallActive);
}

void ATreapTile::ServerSetSystemActive(bool bActive)
{
    if (!HasAuthority()) return;

    bSystemActive = bActive;

    // OFF면 벽도 같이 꺼버림
    if (!bSystemActive)
    {
        bWallActive = false;
        GetWorldTimerManager().ClearTimer(AutoDisableTimer);
    }

    // 서버 즉시 반영
    OnRep_SystemActive();
    OnRep_WallActive();
}

void ATreapTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATreapTile, bSystemActive);
    DOREPLIFETIME(ATreapTile, bWallActive);
}
