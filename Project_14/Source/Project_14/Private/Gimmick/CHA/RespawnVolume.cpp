#include "Gimmick/CHA/RespawnVolume.h"
#include "Gimmick/CHA/RespawnArea.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

ARespawnVolume::ARespawnVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    // 멀티에서 서버/클라 모두 존재하도록
    bReplicates = true;
    SetReplicateMovement(false);

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(Box);

    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetGenerateOverlapEvents(true);

    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARespawnVolume::BeginPlay()
{
    Super::BeginPlay();

    // 서버에서만 오버랩 처리 (레플 정석)
    if (HasAuthority())
    {
        Box->OnComponentBeginOverlap.AddDynamic(this, &ARespawnVolume::OnBoxBeginOverlap);
    }
}

void ARespawnVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !OtherActor) return;

    ACharacter* Char = Cast<ACharacter>(OtherActor);
    if (!Char) return;

    // 플레이어만(컨트롤러 가진 Pawn만) 리스폰 시키고 싶으면 유지
    if (!Char->GetController()) return;

    const float Now = GetWorld()->GetTimeSeconds();
    float& LastTime = LastRespawnTime.FindOrAdd(Char);
    if (Now - LastTime < CooldownSeconds) return;
    LastTime = Now;

    TeleportRespawn(Char);
}

void ARespawnVolume::TeleportRespawn(AActor* Victim)
{
    if (!HasAuthority() || !Victim) return;

    if (!RespawnArea)
    {
        UE_LOG(LogTemp, Warning, TEXT("RespawnVolume: RespawnArea is not set!"));
        return;
    }

    const FTransform SpawnTM = RespawnArea->GetSpawnTransform();
    const FVector TargetLoc = SpawnTM.GetLocation();
    const FRotator TargetRot = SpawnTM.Rotator();

    if (ACharacter* Char = Cast<ACharacter>(Victim))
    {
        if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
        {
            Move->StopMovementImmediately();
        }

        // 서버 텔레포트 -> 이동이 클라에 자동 동기화
        Char->TeleportTo(TargetLoc, TargetRot, false, true);

        if (AController* C = Char->GetController())
        {
            C->SetControlRotation(TargetRot);
        }
    }
    else
    {
        Victim->SetActorLocationAndRotation(TargetLoc, TargetRot, false, nullptr, ETeleportType::TeleportPhysics);
    }
}
