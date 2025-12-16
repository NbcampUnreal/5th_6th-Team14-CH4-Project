#include "Gimmick/MovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;              
	SetReplicateMovement(true);      

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		StartLocation = GetActorLocation();
		CurrentMovingSpeed = MovingSpeed;
	}
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ⭐ 서버에서만 이동/회전 계산
	if (!HasAuthority())
		return;

	MoveActor(DeltaTime);
	RotateActor(DeltaTime);
}


void AMovingPlatform::MoveActor(float DeltaTime)
{
	if (CurrentMovingSpeed.IsNearlyZero() || MovedDistance <= 0.f)
	{
		return;
	}

	const FVector Delta = CurrentMovingSpeed * DeltaTime;
	const FVector CurrentLocation = GetActorLocation();
	FVector NextLocation = CurrentLocation + Delta;

	const float DistFromStart = FVector::Dist(StartLocation, CurrentLocation);
	const float DistAfterMove = FVector::Dist(StartLocation, NextLocation);

	if (DistAfterMove > MovedDistance)
	{
		const FVector Dir = CurrentMovingSpeed.GetSafeNormal();
		const float Overshoot = DistAfterMove - MovedDistance;

		NextLocation -= Dir * Overshoot;
		SetActorLocation(NextLocation);

		StartLocation = NextLocation;
		CurrentMovingSpeed = -CurrentMovingSpeed;
	}
	else
	{
		SetActorLocation(NextLocation);
	}
}

void AMovingPlatform::RotateActor(float DeltaTime)
{
	AddActorLocalRotation(RotationSpeed * DeltaTime);
}


void AMovingPlatform::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMovingPlatform, StartLocation);
	DOREPLIFETIME(AMovingPlatform, CurrentMovingSpeed);
}
