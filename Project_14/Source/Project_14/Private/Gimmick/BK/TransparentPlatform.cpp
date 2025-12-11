#include "Gimmick/BK/TransparentPlatform.h"
#include "Components/StaticMeshComponent.h"

ATransparentPlatform::ATransparentPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    RootComponent = PlatformMesh;

    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ATransparentPlatform::BeginPlay()
{
    Super::BeginPlay();
    bIsVisible = true;
    PlatformMesh->SetVisibility(true);
}

void ATransparentPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Timer += DeltaTime;

    if (bIsVisible && Timer >= VisibleTime)
    {
        bIsVisible = false;
        Timer = 0.f;

        PlatformMesh->SetVisibility(false);     
        PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    else if (!bIsVisible && Timer >= InvisibleTime)
    {
        Timer = 0.f;

        if (bLoop)        
        {
            bIsVisible = true;
            PlatformMesh->SetVisibility(true);
        }
    }
}


