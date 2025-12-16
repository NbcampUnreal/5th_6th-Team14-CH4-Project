#include "Gimmick/CHA/Button_CHA.h"
#include "Gimmick/CHA/MovingFloor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

AButton_CHA::AButton_CHA()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->InitBoxExtent(FVector(80.f, 80.f, 80.f));

    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AButton_CHA::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 서버에서만 바인딩/탐색
    if (!HasAuthority())
        return;

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AButton_CHA::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AButton_CHA::OnOverlapEnd);

    TargetFloor = nullptr;

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AMovingFloor> It(World); It; ++It)
        {
            AMovingFloor* Floor = *It;
            if (Floor && Floor->FloorID == TargetFloorID)
            {
                TargetFloor = Floor;
                break;
            }
        }
    }
}

void AButton_CHA::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (!OtherActor || !TargetFloor) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    TargetFloor->SetMoving(true);
}

void AButton_CHA::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!HasAuthority()) return;
    if (!OtherActor || !TargetFloor) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    TargetFloor->SetMoving(false);
}
