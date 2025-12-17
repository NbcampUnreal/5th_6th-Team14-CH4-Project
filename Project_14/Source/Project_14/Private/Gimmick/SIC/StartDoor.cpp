// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/SIC/StartDoor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AStartDoor::AStartDoor()
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
void AStartDoor::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = SceneRoot->GetRelativeLocation();
	TargetLocation = StartLocation + ExtendedOffset;
}
void AStartDoor::TogglePush()
{
	if (HasAuthority())
	{
		TogglePush_Internal();
	}
	else
	{
		Server_TogglePush();
	}
}
void AStartDoor::ToggleJump()
{
	if (HasAuthority())
	{
		ToggleJump_Internal();
	}
	else
	{
		Server_ToggleJump();
	}
}
void AStartDoor::Server_TogglePush_Implementation()
{
	TogglePush_Internal();
}
void AStartDoor::Server_ToggleJump_Implementation()
{
	ToggleJump_Internal();
}
void AStartDoor::TogglePush_Internal()
{
	bHasPushActivator = true;
}
void AStartDoor::ToggleJump_Internal()
{
	bHasJumpActivator = true;
}
// Called every frame
void AStartDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority()) return;
	if(bHasPushActivator && bHasJumpActivator)
	{
		FVector Current = SceneRoot->GetRelativeLocation();
		FVector NewLocation = FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

		SceneRoot->SetRelativeLocation(NewLocation);
		if (FVector::Dist(Current, TargetLocation) <= 1.0f)
		{
			SceneRoot->SetRelativeLocation(TargetLocation);
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
		}
	}
}


void AStartDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStartDoor, StartLocation);
	DOREPLIFETIME(AStartDoor, TargetLocation);
	DOREPLIFETIME(AStartDoor, bHasPushActivator);
	DOREPLIFETIME(AStartDoor, bHasJumpActivator);
}