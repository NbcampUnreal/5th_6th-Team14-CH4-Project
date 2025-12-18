#include "Gimmick/CHA/PressurePlate.h"
#include "Gimmick/CHA/TimerFloorButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

APressurePlate::APressurePlate()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(false);

    PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
    SetRootComponent(PlateMesh);
    PlateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Trigger->SetupAttachment(RootComponent);
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetGenerateOverlapEvents(true);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    Trigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void APressurePlate::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 오버랩은 서버에서만 처리
    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnBeginOverlap);
    }
}

void APressurePlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor || !ButtonRef) return;

    if (!RequiredTag.IsNone() && !OtherActor->ActorHasTag(RequiredTag))
        return;

    ButtonRef->LockFloor();
}
