#include "Gimmick/BK/BallSpawner.h"
#include "Gimmick/BK/ObstacleBall.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABallSpawner::ABallSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABallSpawner::BeginPlay()
{
	Super::BeginPlay();

	StartSpawn();

	if (BallClass == nullptr)
	{
		BallClass = AObstacleBall::StaticClass(); 
	}

}

void ABallSpawner::SpawnBall()
{
	FVector Location = GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AObstacleBall* Ball = GetWorld()->SpawnActor<AObstacleBall>(BallClass, Location, Rotation, Params);

	if (Ball)
	{
		Ball->SetLifeSpan(BallLifeTime); 
	}
}

void ABallSpawner::StartSpawn()
{
	if (bIsSpawning) return;	
	bIsSpawning = true;

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ABallSpawner::SpawnBall,
		SpawnInterval,
		true
	);
}

void ABallSpawner::StopSpawn()
{
	if (!bIsSpawning) return;	
	bIsSpawning = false;

	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}



