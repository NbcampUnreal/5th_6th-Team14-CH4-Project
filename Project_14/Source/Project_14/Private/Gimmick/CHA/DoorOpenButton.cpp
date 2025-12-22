#include "Gimmick/CHA/DoorOpenButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

ADoorOpenButton::ADoorOpenButton()
{
    PrimaryActorTick.bCanEverTick = false;

    // 버튼 액터는 복제되어야 bOpened가 클라로 감
    bReplicates = true;
    SetReplicateMovement(false);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    // 버튼은 밟히도록 Block
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ButtonMesh->SetCollisionResponseToAllChannels(ECR_Block);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    // 윗면에 얇게 감지 (원하면 수치 조절)
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

    // ✅ 서버만 오버랩 처리 (RPC 소유권 문제 방지)
    if (HasAuthority())
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorOpenButton::OnOverlapBegin);
    }

    // 혹시 맵/복제 타이밍으로 이미 열려있다면 적용
    if (bOpened)
    {
        ApplyOpenedState();
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
    if (!HasAuthority()) return;
    if (!OtherActor) return;

    // 플레이어(Pawn)만
    if (!Cast<APawn>(OtherActor)) return;

    if (bPressOnce && bOpened)
        return;

    PressOnServer(OtherActor);
}

void ADoorOpenButton::PressOnServer(AActor* PressingActor)
{
    if (!HasAuthority()) return;

    // 이미 열렸으면 무시
    if (bOpened)
        return;

    // ✅ 서버에서 true로 세팅 → 클라로 복제됨 → OnRep_Opened 실행
    bOpened = true;

    // 서버 자신도 바로 적용
    ApplyOpenedState();
}

void ADoorOpenButton::OnRep_Opened()
{
    // 클라에서 bOpened가 true로 복제되면 실행됨
    if (bOpened)
    {
        ApplyOpenedState();
    }
}

void ADoorOpenButton::ApplyOpenedState()
{
    auto HideAndDisable = [](AActor* Target)
        {
            if (!IsValid(Target)) return;

            // "사라진 것처럼"
            Target->SetActorHiddenInGame(true);

            // 길 뚫리게(충돌 제거)
            Target->SetActorEnableCollision(false);

            // 필요시 틱도 끔
            Target->SetActorTickEnabled(false);
        };

    HideAndDisable(TargetCube1);
    HideAndDisable(TargetCube2);
}

void ADoorOpenButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoorOpenButton, bOpened);
}
