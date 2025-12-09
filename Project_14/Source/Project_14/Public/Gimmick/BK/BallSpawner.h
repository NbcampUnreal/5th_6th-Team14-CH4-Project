#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallSpawner.generated.h"

UCLASS()
class PROJECT_14_API ABallSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABallSpawner();

protected:
	virtual void BeginPlay() override;

	void SpawnBall();

public:

	UFUNCTION(BlueprintCallable)
	void StartSpawn();

	UFUNCTION(BlueprintCallable)
	void StopSpawn();

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float SpawnInterval = 2.0f; 

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float BallLifeTime = 7.5f; 

	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	TSubclassOf<class AObstacleBall> BallClass;

	FTimerHandle SpawnTimerHandle;
	bool bIsSpawning = false;
};
