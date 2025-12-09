#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StopBallSpawnButton.generated.h"

UCLASS()
class PROJECT_14_API AStopBallSpawnButton : public AActor
{
	GENERATED_BODY()
	
public:	
	AStopBallSpawnButton();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ButtonMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Spawner")
	class ABallSpawner* TargetSpawner;

};
