// WallToggleButton.cpp
#include "Gimmick/CHA/WallToggleButton.h"
#include "Gimmick/CHA/WallToggle.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"

AWallToggleButton::AWallToggleButton()
{
    PrimaryActorTick.bCanEverTick = false;

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    SetRootComponent(ButtonMesh);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);

    // 플레이어만 겹치도록 설정
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWallToggleButton::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWallToggleButton::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AWallToggleButton::OnOverlapEnd);
}

void AWallToggleButton::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar) return;

    bPlayerOnButton = true;

    // 이미 벽 끄기 타이머가 걸려 있으면 취소
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HideWallTimerHandle);
    }

    // 버튼 위에 올라왔으니 벽 ON
    if (TargetWall)
    {
        TargetWall->SetWallEnabled(true);
    }
}

void AWallToggleButton::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (!PlayerChar) return;

    bPlayerOnButton = false;

    // 1초 뒤에 벽 끄기 타이머 설정
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            HideWallTimerHandle,
            this,
            &AWallToggleButton::HideWall,
            2.0f,   // 1초 후
            false   // 반복 X
        );
    }
}

void AWallToggleButton::HideWall()
{
    // 타이머가 발동했을 때, 만약 그 사이에 다시 올라왔다면 벽 유지
    if (bPlayerOnButton)
    {
        return;
    }

    if (TargetWall)
    {
        TargetWall->SetWallEnabled(false);
    }
}
