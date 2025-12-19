#include "Gimmick/SIC/DragonBridgeJump.h"
#include "Gimmick/SIC/MakerBlockJump.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ADragonBridgeJump::ADragonBridgeJump()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	bAlwaysRelevant = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(SceneRoot);
}

void ADragonBridgeJump::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	// 처음엔 멈춰 있음
	SetActorTickEnabled(false);

	// 경로 수집 (기존 로직 유지)
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AMakerBlockJump::StaticClass(),
		Found
	);

	Targets.SetNum(MaxTargetNum);

	for (AActor* A : Found)
	{
		AMakerBlockJump* Marker = Cast<AMakerBlockJump>(A);
		if (!Marker)
			continue;

		if (Targets.IsValidIndex(Marker->TargetIndex))
		{
			Targets[Marker->TargetIndex] = Marker->GetActorLocation();
		}
	}
}

void ADragonBridgeJump::ToggleState()
{
	if (bHasStarted)
		return;

	bHasStarted = true;
	bIsStopped = false;

	if (HasAuthority())
	{
		ToggleState_Internal();
	}
}

void ADragonBridgeJump::ToggleState_Internal()
{
	UE_LOG(LogTemp, Warning, TEXT("[DragonBridgeJump] ToggleState"));

	SetActorLocation(StartLocation);

	TargetNum = 0;
	bIsEnd = false;

	if (Targets.Num() > 0)
	{
		TargetLocation = Targets[0];
	}

	SetActorTickEnabled(true);
}

void ADragonBridgeJump::StopBridge()
{
	if (HasAuthority())
	{
		StopBridge_Internal();
	}
	else
	{
		Server_StopBridge();
	}
}

void ADragonBridgeJump::Server_StopBridge_Implementation()
{
	StopBridge_Internal();
}

void ADragonBridgeJump::StopBridge_Internal()
{
	if (bIsStopped)
		return;

	UE_LOG(LogTemp, Warning, TEXT("[DragonBridgeJump] StopBridge"));

	bIsStopped = true;
	Multicast_ApplyStop();
}

void ADragonBridgeJump::Multicast_ApplyStop_Implementation()
{
	bIsStopped = true;
	SetActorTickEnabled(false);
}

void ADragonBridgeJump::NextTarget()
{
	TargetNum++;

	if (TargetNum >= Targets.Num())
	{
		bIsEnd = true;
		SetActorTickEnabled(false);
		return;
	}

	TargetLocation = Targets[TargetNum];
}

void ADragonBridgeJump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStopped || bIsEnd)
		return;

	FVector Current = GetActorLocation();
	FVector NewLocation =
		FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NewLocation);

	if (FVector::Dist(Current, TargetLocation) < 1.f)
	{
		NextTarget();
	}
}

void ADragonBridgeJump::OnRep_TargetNum()
{
}

void ADragonBridgeJump::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADragonBridgeJump, StartLocation);
	DOREPLIFETIME(ADragonBridgeJump, TargetNum);
	DOREPLIFETIME(ADragonBridgeJump, TargetLocation);
	DOREPLIFETIME(ADragonBridgeJump, bIsEnd);
	DOREPLIFETIME(ADragonBridgeJump, bIsStopped);
}
