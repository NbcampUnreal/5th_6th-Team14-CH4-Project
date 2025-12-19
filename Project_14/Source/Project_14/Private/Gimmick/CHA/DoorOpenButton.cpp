#include "Gimmick/CHA/DoorOpenButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"

ADoorOpenButton::ADoorOpenButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(false);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    // 발판(블록 통과 문제 있으면 여기 먼저 체크)
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ButtonMesh->SetCollisionResponseToAllChannels(ECR_Block);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    // ✅ 윗면에 얇게(원하면 에디터에서 조정 가능)
    TriggerBox->SetBoxExtent(FVector(45.f, 45.f, 5.f));
    TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, 25.f));

    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ADoorOpenButton::BeginPlay()
{
    Super::BeginPlay();

    // ✅ 핵심: 서버만 오버랩 처리 (RPC 필요 없음)
    if (HasAuthority())
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorOpenButton::OnOverlapBegin);
    }
}

void ADoorOpenButton::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // BeginPlay에서 서버만 바인딩했으니 여기서는 서버라고 봐도 됨
    if (!HasAuthority())
        return;

    if (!OtherActor)
        return;

    // 플레이어(Pawn)만
    if (!Cast<APawn>(OtherActor))
        return;

    if (bPressOnce && bPressed)
        return;

    PressOnServer(OtherActor);
}

void ADoorOpenButton::PressOnServer(AActor* PressingActor)
{
    if (!HasAuthority())
        return;

    bPressed = true;

    // ✅ 지정한 큐브 2개 삭제
    if (IsValid(TargetCube1))
    {
        TargetCube1->Destroy();
    }

    if (IsValid(TargetCube2))
    {
        TargetCube2->Destroy();
    }
}
