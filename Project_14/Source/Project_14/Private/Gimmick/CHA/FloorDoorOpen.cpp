#include "Gimmick/CHA/FloorDoorOpen.h"

#include "Gimmick/CHA/TimerFloorButton.h"
#include "Gimmick/CHA/RaiseDoor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AFloorDoorOpen::AFloorDoorOpen()
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

void AFloorDoorOpen::BeginPlay()
{
    Super::BeginPlay();

    // 서버에서만 처리(네 프로젝트 스타일 유지)
    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &AFloorDoorOpen::OnBeginOverlap);
    }
}

void AFloorDoorOpen::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor) return;
    if (!Cast<APawn>(OtherActor)) return;

    if (bOneTimeUse && bUsed) return;
    bUsed = true;

    // 1) 25초 타이머 캔슬 + 바닥 유지
    if (FloorTimerButtonRef)
    {
        FloorTimerButtonRef->LockFloor();
    }

    // 2) 문 열기
    if (TargetDoor)
    {
        TargetDoor->OpenDoor();
    }
}
