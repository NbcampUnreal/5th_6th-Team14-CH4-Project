#include "Gimmick/PortalActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"        
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameManager/ProjectGameStateBase.h"

APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(Root);

	PortalFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalFX"));
	PortalFX->SetupAttachment(Root);

	PortalFX->SetAutoActivate(true);
	PortalFX->SetIsReplicated(false);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this, &APortalActor::OnOverlapBegin);
	}
}

void APortalActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || !LinkedPortal)
		return;

	ServerTeleport(Character);
}

void APortalActor::ServerTeleport_Implementation(ACharacter* Character)
{
	if (!Character || !LinkedPortal)
		return;

	float* LastTime = LastTeleportTime.Find(Character);
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (LastTime && CurrentTime - *LastTime < TeleportCooldown)
		return;

	// 내 포탈 쿨타임 기록
	LastTeleportTime.Add(Character, CurrentTime);

	// 도착지(LinkedPortal)에도 쿨타임 기록해서 "도착하자마자" 방지
	LinkedPortal->LastTeleportTime.Add(Character, CurrentTime);

	// 텔포 성공 여부 받아서 성공했을 때만 통과 처리
	const bool bTeleported = TeleportCharacter(Character);
	if (bTeleported)
	{
		RegisterPortalPass(Character);
	}
}

bool APortalActor::TeleportCharacter(ACharacter* Character) // 반환형 bool
{
	if (!Character || !LinkedPortal)
		return false;

	FVector TargetLocation =
		LinkedPortal->GetActorLocation() +
		LinkedPortal->GetActorForwardVector() * 120.f;

	FRotator TargetRotation = LinkedPortal->GetActorRotation();

	const bool bSuccess = Character->TeleportTo(TargetLocation, TargetRotation, false, true);

	UE_LOG(LogTemp, Warning,
		TEXT("%s Teleported to %s (Success=%d)"),
		*GetNameSafe(Character),
		*GetNameSafe(LinkedPortal),
		bSuccess ? 1 : 0);

	return bSuccess;
}

// 두 명 모두 통과했을 때만 1회 클리어 카운트 증가
void APortalActor::RegisterPortalPass(ACharacter* Character)
{
	if (!HasAuthority())
		return;

	// 출구 포탈은 false로 해서 중복 증가 방지 권장
	if (!bCountsForClear)
		return;

	// 이미 클리어 처리했고 반복 허용 안 하면 더 이상 증가 X
	if (bClearedThisPortal && !bAllowRepeatClear)
		return;

	APlayerState* PS = Character ? Character->GetPlayerState() : nullptr;
	if (!PS)
		return;

	const int32 Before = PassedPlayers.Num();
	const bool bAlready = PassedPlayers.Contains(PS);

	if (!bAlready)
	{
		PassedPlayers.Add(PS);
	}

	const int32 After = PassedPlayers.Num();

	UE_LOG(LogTemp, Warning,
		TEXT("[Portal] Pass %s: %s (Before=%d After=%d /%d)"),
		bAlready ? TEXT("Repeat") : TEXT("Added"),
		*GetNameSafe(Character),
		Before, After, RequiredPlayersToClear);

	// 두 명 모이면 딱 1번만 클리어 카운트 증가
	if (!bClearedThisPortal && After >= RequiredPlayersToClear)
	{
		if (AProjectGameStateBase* GS = GetWorld()->GetGameState<AProjectGameStateBase>())
		{
			GS->OnMapCleared();

			UE_LOG(LogTemp, Warning,
				TEXT(" CLEARED! ClearCount=%d"),
				GS->ClearedMapCount);
		}

		bClearedThisPortal = true;

		// 반복 허용이면 다음 사이클을 위해 초기화
		if (bAllowRepeatClear)
		{
			bClearedThisPortal = false;
			PassedPlayers.Reset();
		}
	}
}
