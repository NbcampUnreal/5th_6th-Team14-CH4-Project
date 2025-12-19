#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StopBridgeJump.generated.h"

UCLASS()
class PROJECT_14_API AStopBridgeJump : public AActor
{
	GENERATED_BODY()

public:
	AStopBridgeJump();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TriggerMesh;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
