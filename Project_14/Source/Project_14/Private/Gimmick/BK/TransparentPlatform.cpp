#include "Gimmick/BK/TransparentPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ATransparentPlatform::ATransparentPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;                 
    SetReplicateMovement(false);       

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);

    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ATransparentPlatform::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        bIsVisible = true;
        Timer = 0.f;
    }

    OnRep_VisibilityState();
}

void ATransparentPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

   if (!HasAuthority())
        return;

    Timer += DeltaTime;

    if (bIsVisible && Timer >= VisibleTime)
    {
        bIsVisible = false;
        Timer = 0.f;
        OnRep_VisibilityState();
    }
    else if (!bIsVisible && Timer >= InvisibleTime)
    {
        Timer = 0.f;

        if (bLoop)
        {
            bIsVisible = true;
            OnRep_VisibilityState();
        }
    }
}

void ATransparentPlatform::OnRep_VisibilityState()
{
    PlatformMesh->SetVisibility(bIsVisible);
    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void ATransparentPlatform::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATransparentPlatform, bIsVisible);
}
