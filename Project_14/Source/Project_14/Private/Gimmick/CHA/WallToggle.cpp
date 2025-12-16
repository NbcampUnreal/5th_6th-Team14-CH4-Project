#include "Gimmick/CHA/WallToggle.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AWallToggle::AWallToggle()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    SetRootComponent(WallMesh);

    ApplyWallEnabled(false);
    bWallEnabled = false;
}

void AWallToggle::ApplyWallEnabled(bool bEnable)
{
    if (!WallMesh) return;

    WallMesh->SetVisibility(bEnable);
    WallMesh->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void AWallToggle::OnRep_WallEnabled()
{
    ApplyWallEnabled(bWallEnabled);
}

void AWallToggle::SetWallEnabled(bool bEnable)
{
    // ✅ 서버에서만 상태 변경
    if (!HasAuthority())
        return;

    bWallEnabled = bEnable;
    ApplyWallEnabled(bWallEnabled);
}

void AWallToggle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWallToggle, bWallEnabled);
}
