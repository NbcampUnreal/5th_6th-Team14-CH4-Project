// DoorOpenButton.cpp

#include "Gimmick/CHA/DoorOpenButton.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

ADoorOpenButton::ADoorOpenButton()
{
    PrimaryActorTick.bCanEverTick = false;

    // 버튼 메쉬 생성
    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    // 트리거 박스 생성
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->InitBoxExtent(FVector(80.f, 80.f, 40.f));

    // 오버랩만 받도록 설정
    TriggerBox->SetGenerateOverlapEvents(true);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ADoorOpenButton::BeginPlay()
{
    Super::BeginPlay();

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
    // 이미 작동했거나, 타겟이 없거나, OtherActor 가 없으면 무시
    if (bAlreadyActivated || TargetWalls.Num() == 0 || !OtherActor)
    {
        return;
    }

    // 플레이어만 반응
    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }

    // 배열에 들어있는 모든 벽/문 액터들을 순회하면서 처리
    for (AActor* Wall : TargetWalls)
    {
        if (!Wall) continue;

        UE_LOG(LogTemp, Warning,
            TEXT("[DoorOpenButton] 버튼 발동! 타겟 벽 액터 Destroy 시도: %s"),
            *Wall->GetName());

        // 1) 컴포넌트 가시성/충돌 끄기
        TArray<UActorComponent*> Components;
        Wall->GetComponents(UPrimitiveComponent::StaticClass(), Components);

        for (UActorComponent* Comp : Components)
        {
            if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
            {
                Prim->SetVisibility(false, true);
                Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }

        // 2) 액터 파괴
        Wall->Destroy();
    }

    bAlreadyActivated = true; // 한 번만 동작
}

void ADoorOpenButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    // 문은 Destroy 되어버렸으니 여기서는 딱히 할 일 없음
}
