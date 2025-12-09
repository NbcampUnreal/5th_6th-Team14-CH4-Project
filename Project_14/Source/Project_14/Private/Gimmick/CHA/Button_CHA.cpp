#include "Gimmick/CHA/Button_CHA.h"
#include "Gimmick/CHA/MovingFloor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"   // TActorIterator

AButton_CHA::AButton_CHA()
{
    PrimaryActorTick.bCanEverTick = false;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    // 🔹 트리거 박스 기본 크기 (버튼보다 꽤 크게)
    TriggerBox->InitBoxExtent(FVector(80.f, 80.f, 80.f));

    // 🔹 무조건 오버랩 이벤트 켜기
    TriggerBox->SetGenerateOverlapEvents(true);

    // 🔹 쿼리만 (충돌은 안 막고, 겹침만 체크)
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 🔹 이 박스의 ObjectType은 WorldDynamic 으로
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);

    // 🔹 기본은 전부 무시
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);

    // 🔹 Pawn(플레이어)하고만 Overlap
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AButton_CHA::BeginPlay()
{
    Super::BeginPlay();

    // 🔹 1) Overlap 바인딩
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AButton_CHA::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AButton_CHA::OnOverlapEnd);

    // 🔹 2) 월드에서 FloorID가 같은 MovingFloor를 자동으로 찾기
    TargetFloor = nullptr;

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AMovingFloor> It(World); It; ++It)
        {
            AMovingFloor* Floor = *It;
            if (Floor && Floor->FloorID == TargetFloorID)
            {
                TargetFloor = Floor;

                // ▶ 버튼이 어느 바닥을 찾았는지 출력
                UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] FloorID '%s' 에 해당하는 바닥 찾음: %s"),
                    *TargetFloorID.ToString(),
                    *Floor->GetName());

                break;
            }
        }
    }

    if (!TargetFloor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] FloorID '%s' 에 해당하는 바닥을 찾지 못했습니다."),
            *TargetFloorID.ToString());
    }
}

void AButton_CHA::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // ▶ 일단 누가 밟았는지 로그부터 찍기
    UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] OnOverlapBegin: OtherActor = %s"),
        OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

    if (!OtherActor || !TargetFloor || bAlreadyActivated)
    {
        return;
    }

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] Overlap은 들어왔지만, 플레이어가 아님"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] 플레이어가 버튼 위에 올라옴 -> StartMove 호출"));

    TargetFloor->StartMove();
    bAlreadyActivated = true;
}

void AButton_CHA::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (Cast<ACharacter>(OtherActor))
    {
        bAlreadyActivated = false;
    }
}
