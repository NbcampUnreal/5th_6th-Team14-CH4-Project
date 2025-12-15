#include "Gimmick/BK/BallSpawner.h"
#include "Gimmick/BK/ObstacleBall.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ABallSpawner::ABallSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;  
}

void ABallSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (BallClass == nullptr)
		{
			BallClass = AObstacleBall::StaticClass();
		}

		StartSpawn(); 
	}
}

void ABallSpawner::SpawnBall()
{
	if (!HasAuthority())
		return;

	FVector Location = GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AObstacleBall* Ball =
		GetWorld()->SpawnActor<AObstacleBall>(BallClass, Location, Rotation, Params);

	if (Ball)
	{
		Ball->SetLifeSpan(BallLifeTime);
	}
}

void ABallSpawner::StartSpawn()
{
	if (HasAuthority())
	{
		ServerStartSpawn();
	}
	else
	{
		ServerStartSpawn(); // RPC
	}
}

void ABallSpawner::StopSpawn()
{
	if (HasAuthority())
	{
		ServerStopSpawn();
	}
	else
	{
		ServerStopSpawn(); // RPC
	}
}

void ABallSpawner::ServerStartSpawn_Implementation()
{
	if (bIsSpawning)
		return;

	bIsSpawning = true;

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ABallSpawner::SpawnBall,
		SpawnInterval,
		true
	);

	OnRep_SpawnState();
}

void ABallSpawner::ServerStopSpawn_Implementation()
{
	if (!bIsSpawning)
		return;

	bIsSpawning = false;

	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	OnRep_SpawnState();
}

void ABallSpawner::OnRep_SpawnState()
{
	// 클라 연출용 (UI, 사운드 등)
}

void ABallSpawner::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABallSpawner, bIsSpawning);
}
