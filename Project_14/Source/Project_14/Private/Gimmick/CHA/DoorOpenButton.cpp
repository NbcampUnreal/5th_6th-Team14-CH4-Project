#include "Gimmick/CHA/DoorOpenButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

ADoorOpenButton::ADoorOpenButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(false);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    // ✅ 버튼 메쉬가 Pawn을 막아서 오버랩이 안 터지는 경우가 많아서 기본은 NoCollision 추천
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

    // ✅ 서버/클라 둘 다 바인딩 (오버랩 감지는 어디서든 일어날 수 있음)
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
    if (bAlreadyActivated || TargetWalls.Num() == 0 || !OtherActor)
        return;

    // ✅ Character가 아니라 Pawn이어도 허용 (더 안전)
    if (!Cast<APawn>(OtherActor))
        return;

    // ✅ 클라에서 밟으면 서버 RPC 요청
    if (!HasAuthority())
    {
        ServerActivate(OtherActor);
        return;
    }

    // ✅ 서버라면 바로 실행
    ActivateOnServer(OtherActor);
}

void ADoorOpenButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
}

void ADoorOpenButton::ServerActivate_Implementation(AActor* Activator)
{
    // 이 함수는 서버에서 실행됨
    ActivateOnServer(Activator);
}

void ADoorOpenButton::ActivateOnServer(AActor* Activator)
{
    if (!HasAuthority())
        return;

    if (bAlreadyActivated || TargetWalls.Num() == 0 || !Activator)
        return;

    if (!Cast<APawn>(Activator))
        return;

    for (AActor* Wall : TargetWalls)
    {
        if (!Wall) continue;
        Wall->Destroy(); // ✅ 서버 Destroy -> 복제되는 액터면 클라에서도 같이 사라짐
    }

    bAlreadyActivated = true;
}

void ADoorOpenButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoorOpenButton, bAlreadyActivated);
}
