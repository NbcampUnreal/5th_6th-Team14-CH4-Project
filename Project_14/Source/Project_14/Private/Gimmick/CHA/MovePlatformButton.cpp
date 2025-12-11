// MovePlatformButton.cpp

#include "Gimmick/CHA/MovePlatformButton.h"
#include "Gimmick/CHA/MoveablePlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

AMovePlatformButton::AMovePlatformButton()
{
    PrimaryActorTick.bCanEverTick = true;   // 버튼은 Tick 사용

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

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMovePlatformButton::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMovePlatformButton::OnOverlapEnd);
}

void AMovePlatformButton::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPressed && TargetPlatform)
    {
        // 버튼 위에 서 있는 동안만 한 프레임씩 이동
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
        bPressed = true;   // 올라오면 이동 시작
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
        bPressed = false;  // 내려가면 이동 정지
    }
}
