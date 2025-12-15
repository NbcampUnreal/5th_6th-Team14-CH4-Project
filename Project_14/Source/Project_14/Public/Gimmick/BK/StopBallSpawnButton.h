#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StopBallSpawnButton.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ABallSpawner;

UCLASS()
class PROJECT_14_API AStopBallSpawnButton : public AActor
{
	GENERATED_BODY()

public:
	AStopBallSpawnButton();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

    UFUNCTION(Server, Reliable)
	void ServerPress();

	UFUNCTION(Server, Reliable)
	void ServerRelease();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spawner")
	ABallSpawner* TargetSpawner;

	UPROPERTY(ReplicatedUsing = OnRep_ButtonState)
	bool bIsPressed = false;

	UFUNCTION()
	void OnRep_ButtonState();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
