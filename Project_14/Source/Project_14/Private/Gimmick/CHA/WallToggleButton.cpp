#include "Gimmick/CHA/WallToggleButton.h"
#include "Gimmick/CHA/WallToggle.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

AWallToggleButton::AWallToggleButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWallToggleButton::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 서버만 바인딩
    if (!HasAuthority())
        return;

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWallToggleButton::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AWallToggleButton::OnOverlapEnd);
}

void AWallToggleButton::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    bPlayerOnButton = true;
    GetWorldTimerManager().ClearTimer(HideWallTimerHandle);

    if (TargetWall)
    {
        TargetWall->SetWallEnabled(true);
    }
}

void AWallToggleButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!HasAuthority()) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    bPlayerOnButton = false;

    GetWorldTimerManager().SetTimer(
        HideWallTimerHandle,
        this,
        &AWallToggleButton::HideWall,
        HideDelay,
        false
    );
}

void AWallToggleButton::HideWall()
{
    if (!HasAuthority()) return;
    if (bPlayerOnButton) return;

    if (TargetWall)
    {
        TargetWall->SetWallEnabled(false);
    }
}
