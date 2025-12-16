#include "Gimmick/CHA/DoorOpenButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

ADoorOpenButton::ADoorOpenButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->InitBoxExtent(FVector(80.f, 80.f, 40.f));

    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ADoorOpenButton::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 서버만 바인딩
    if (!HasAuthority())
        return;

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorOpenButton::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoorOpenButton::OnOverlapEnd);
}

void ADoorOpenButton::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADoorOpenButton::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    if (bAlreadyActivated || TargetWalls.Num() == 0 || !OtherActor)
        return;

    if (!Cast<ACharacter>(OtherActor))
        return;

    for (AActor* Wall : TargetWalls)
    {
        if (!Wall) continue;

        // ✅ 서버 Destroy (타겟 액터 Replicates=true면 클라에도 같이 사라짐)
        Wall->Destroy();
    }

    bAlreadyActivated = true;
}

void ADoorOpenButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
}
