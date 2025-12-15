#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallSpawner.generated.h"

class AObstacleBall;

UCLASS()
class PROJECT_14_API ABallSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABallSpawner();

protected:
	virtual void BeginPlay() override;

	void SpawnBall();

	UFUNCTION(Server, Reliable)
	void ServerStartSpawn();

	UFUNCTION(Server, Reliable)
	void ServerStopSpawn();

protected:
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float SpawnInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float BallLifeTime = 7.5f;

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	TSubclassOf<AObstacleBall> BallClass;

	UPROPERTY(ReplicatedUsing = OnRep_SpawnState)
	bool bIsSpawning = false;

	FTimerHandle SpawnTimerHandle;

	UFUNCTION()
	void OnRep_SpawnState();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void StartSpawn();
	void StopSpawn();
};
