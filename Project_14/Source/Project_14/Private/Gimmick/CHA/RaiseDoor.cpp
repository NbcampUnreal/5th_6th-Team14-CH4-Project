#include "Gimmick/CHA/RaiseDoor.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/World.h"

ARaiseDoor::ARaiseDoor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    SetRootComponent(DoorMesh);
}

void ARaiseDoor::BeginPlay()
{
    Super::BeginPlay();

    ClosedLocation = GetActorLocation();
    OpenLocation = ClosedLocation + FVector(0, 0, OpenHeight);
}

void ARaiseDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARaiseDoor, bIsOpen);
}

void ARaiseDoor::OpenDoor()
{
    if (HasAuthority())
    {
        if (bIsOpen) return;
        bIsOpen = true;
        MulticastStartOpen();
    }
    else
    {
        ServerOpenDoor();
    }
}

void ARaiseDoor::ServerOpenDoor_Implementation()
{
    OpenDoor();
}

void ARaiseDoor::MulticastStartOpen_Implementation()
{
    GetWorldTimerManager().ClearTimer(OpenTimer);

    Elapsed = 0.0f;
    ClosedLocation = GetActorLocation();
    OpenLocation = ClosedLocation + FVector(0, 0, OpenHeight);

    GetWorldTimerManager().SetTimer(OpenTimer, this, &ARaiseDoor::TickOpen, 0.02f, true);
}

void ARaiseDoor::TickOpen()
{
    Elapsed += 0.02f;
    const float Alpha = (OpenDuration <= 0.f) ? 1.f : FMath::Clamp(Elapsed / OpenDuration, 0.f, 1.f);

    SetActorLocation(FMath::Lerp(ClosedLocation, OpenLocation, Alpha));

    if (Alpha >= 1.f)
    {
        GetWorldTimerManager().ClearTimer(OpenTimer);
    }
}
