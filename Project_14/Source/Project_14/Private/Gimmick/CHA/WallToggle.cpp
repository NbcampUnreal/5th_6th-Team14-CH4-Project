// WallToggle.cpp
#include "Gimmick/CHA/WallToggle.h"
#include "Components/StaticMeshComponent.h"

AWallToggle::AWallToggle()
{
    PrimaryActorTick.bCanEverTick = false;

    WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
    SetRootComponent(WallMesh);

    // 🔹 시작 상태: "안 보이고, 충돌 없음" (벽이 없는 상태)
    WallMesh->SetVisibility(false);
    WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWallToggle::SetWallEnabled(bool bEnable)
{
    if (!WallMesh) return;

    // bEnable = true  → 보이기 + 충돌 켜기
    // bEnable = false → 숨기기 + 충돌 끄기
    WallMesh->SetVisibility(bEnable);
    WallMesh->SetCollisionEnabled(
        bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision
    );
}
