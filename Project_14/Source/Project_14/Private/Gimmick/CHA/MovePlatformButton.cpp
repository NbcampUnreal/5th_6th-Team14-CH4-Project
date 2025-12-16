#include "Gimmick/CHA/MovePlatformButton.h"
#include "Gimmick/CHA/MoveablePlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

AMovePlatformButton::AMovePlatformButton()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMovePlatformButton::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 서버만 바인딩
    if (!HasAuthority())
        return;

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMovePlatformButton::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMovePlatformButton::OnOverlapEnd);
}

void AMovePlatformButton::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ✅ 서버만 MoveStep 호출
    if (!HasAuthority())
        return;

    if (bPressed && TargetPlatform)
    {
        TargetPlatform->MoveStep(MoveDirection, DeltaTime);
    }
}

void AMovePlatformButton::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (Cast<ACharacter>(OtherActor))
    {
        bPressed = true;
    }
}

void AMovePlatformButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (Cast<ACharacter>(OtherActor))
    {
        bPressed = false;
    }
}
