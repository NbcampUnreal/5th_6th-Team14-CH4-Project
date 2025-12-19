#include "Gimmick/SIC/DragonBridge.h"
#include "Gimmick/SIC/MakerBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ADragonBridge::ADragonBridge()
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

void ADragonBridge::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	// 처음엔 멈춰 있음
	SetActorTickEnabled(false);

	// 경로 수집 (기존 로직 유지)
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AMakerBlock::StaticClass(),
		Found
	);

	Targets.SetNum(MaxTargetNum);

	for (AActor* A : Found)
	{
		AMakerBlock* Marker = Cast<AMakerBlock>(A);
		if (!Marker)
			continue;

		if (Targets.IsValidIndex(Marker->TargetIndex))
		{
			Targets[Marker->TargetIndex] = Marker->GetActorLocation();
		}
	}
}

void ADragonBridge::ToggleState()
{
	if (bHasStarted)
		return;

	bHasStarted = true;

	if (HasAuthority())
	{
		ToggleState_Internal();
	}
}

void ADragonBridge::ToggleState_Internal()
{
	UE_LOG(LogTemp, Warning, TEXT("[DragonBridge] ToggleState"));

	SetActorLocation(StartLocation);

	TargetNum = 0;
	bIsEnd = false;

	if (Targets.Num() > 0)
	{
		TargetLocation = Targets[0];
	}

	SetActorTickEnabled(true);
}

void ADragonBridge::NextTarget()
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

void ADragonBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsEnd)
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

void ADragonBridge::OnRep_TargetNum()
{
}

void ADragonBridge::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADragonBridge, StartLocation);
	DOREPLIFETIME(ADragonBridge, TargetNum);
	DOREPLIFETIME(ADragonBridge, TargetLocation);
	DOREPLIFETIME(ADragonBridge, bIsEnd);
}
