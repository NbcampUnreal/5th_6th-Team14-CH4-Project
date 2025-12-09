#include "Gimmick/MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);
	
	PrimaryActorTick.bCanEverTick = true;

}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveActor(DeltaTime);
	RotateActor(DeltaTime);

}

void AMovingPlatform::MoveActor(float DeltaTime)
{
    if (MovingSpeed.IsNearlyZero() || MovedDistance <= 0.f)
    {
        return;
    }


    const FVector Delta = MovingSpeed * DeltaTime;
    FVector CurrentLocation = GetActorLocation();
    FVector NextLocation = CurrentLocation + Delta;

    const float DistFromStart = FVector::Dist(StartLocation, CurrentLocation);
    const float DistAfterMove = FVector::Dist(StartLocation, NextLocation);


    if (DistAfterMove > MovedDistance)
    {

        const FVector Dir = MovingSpeed.GetSafeNormal();


        const float Overshoot = DistAfterMove - MovedDistance;


        NextLocation = NextLocation - Dir * Overshoot;


        SetActorLocation(NextLocation);


        StartLocation = NextLocation;
        MovingSpeed = -MovingSpeed;
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

bool AMovingPlatform::ShouldActorReturn() const
{
    return GetDistanceMoved() > MovedDistance;
}

float AMovingPlatform::GetDistanceMoved() const
{
    return FVector::Dist(StartLocation, GetActorLocation());
}

