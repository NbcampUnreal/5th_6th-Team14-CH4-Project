// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/SIC/MovingBlock.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMovingBlock::AMovingBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AMovingBlock::BeginPlay()
{
	Super::BeginPlay();
	const FVector Current = SceneRoot->GetRelativeLocation();
	StartLocation = SceneRoot->GetRelativeLocation();
	if (bIsExtended)
	{
		StartLocation = Current - ExtendedOffset;
	}
	else
	{
		StartLocation = Current;
	}
	TargetLocation = bIsExtended ? StartLocation + ExtendedOffset : StartLocation;
}
void AMovingBlock::ToggleState()
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
void AMovingBlock::Server_ToggleState_Implementation()
{
	ToggleState_Internal();
}
void AMovingBlock::ToggleState_Internal()
{
	bIsExtended = !bIsExtended;

	if (bIsExtended)
		TargetLocation = StartLocation + ExtendedOffset;
	else
		TargetLocation = StartLocation;
}
// Called every frame
void AMovingBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority()) return;

	FVector Current = SceneRoot->GetRelativeLocation();
	FVector NewLocation = FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

	SceneRoot->SetRelativeLocation(NewLocation);
}


void AMovingBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMovingBlock, StartLocation);
	DOREPLIFETIME(AMovingBlock, TargetLocation);
	DOREPLIFETIME(AMovingBlock, bIsExtended);
}