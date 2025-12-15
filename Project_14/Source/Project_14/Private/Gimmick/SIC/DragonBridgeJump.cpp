#include "Gimmick/SIC/DragonBridgeJump.h"
#include "Gimmick/SIC/MakerBlockJump.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ADragonBridgeJump::ADragonBridgeJump()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(SceneRoot);
}

void ADragonBridgeJump::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMakerBlockJump::StaticClass(), Found);

	Targets.SetNum(MaxTargetNum);

	for (AActor* A : Found)
	{
		AMakerBlockJump* Marker = Cast<AMakerBlockJump>(A);
		if (!Marker) continue;

		if (Targets.IsValidIndex(Marker->TargetIndex))
		{
			Targets[Marker->TargetIndex] = Marker->GetActorLocation();
		}
	}
}

void ADragonBridgeJump::ToggleState()
{
	if (HasAuthority())
	{
		ToggleState_Internal();
	}
	else
	{
		Server_ToggleState();
	}
}

void ADragonBridgeJump::Server_ToggleState_Implementation()
{
	ToggleState_Internal();
}

void ADragonBridgeJump::ToggleState_Internal()
{
	SetActorLocation(StartLocation);

	TargetNum = 0;
	bIsEnd = false;
	TargetLocation = Targets[0];

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ADragonBridgeJump::NextTarget()
{
	TargetNum++;

	if (TargetNum >= MaxTargetNum)
	{
		bIsEnd = true;
		return;
	}

	TargetLocation = Targets[TargetNum];
}

void ADragonBridgeJump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;

	FVector Current = GetActorLocation();
	FVector NewLocation =
		FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NewLocation);

	if (FVector::Dist(Current, TargetLocation) < 1.f)
	{
		if (bIsEnd)
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
			return;
		}

		NextTarget();
	}
}

void ADragonBridgeJump::OnRep_TargetNum()
{
}

void ADragonBridgeJump::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADragonBridgeJump, StartLocation);
	DOREPLIFETIME(ADragonBridgeJump, TargetNum);
	DOREPLIFETIME(ADragonBridgeJump, TargetLocation);
	DOREPLIFETIME(ADragonBridgeJump, bIsEnd);
}
