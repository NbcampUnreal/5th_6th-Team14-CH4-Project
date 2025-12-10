// Button_CHA.cpp

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

    // 트리거 박스 기본 크기 (발판보다 넉넉하게)
    TriggerBox->InitBoxExtent(FVector(80.f, 80.f, 80.f));

    // 오버랩 이벤트 강제 활성화
    TriggerBox->SetGenerateOverlapEvents(true);

    // 쿼리만 (충돌 막지는 않고, 겹침만 체크)
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // ObjectType: WorldDynamic
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);

    // 기본은 전부 무시
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);

    // Pawn(플레이어)하고만 Overlap
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AButton_CHA::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 바인딩
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AButton_CHA::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AButton_CHA::OnOverlapEnd);

    // 월드에서 FloorID가 같은 MovingFloor 자동 찾기
    TargetFloor = nullptr;

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AMovingFloor> It(World); It; ++It)
        {
            AMovingFloor* Floor = *It;
            if (Floor && Floor->FloorID == TargetFloorID)
            {
                TargetFloor = Floor;

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
    UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] OnOverlapBegin: OtherActor = %s"),
        OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

    if (!OtherActor || !TargetFloor)
    {
        return;
    }

    // 플레이어만 반응
    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] Overlap은 들어왔지만 플레이어가 아님"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] 플레이어가 버튼 위에 올라옴 -> 바닥 움직임 ON"));

    // 발판 위에 올라오면 바닥 움직임 ON
    TargetFloor->SetMoving(true);
}

void AButton_CHA::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor || !TargetFloor)
    {
        return;
    }

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Button_CHA] 플레이어가 버튼에서 내려감 -> 바닥 움직임 OFF"));

    // 발판에서 내려가면 바닥 움직임 OFF
    TargetFloor->SetMoving(false);
}
