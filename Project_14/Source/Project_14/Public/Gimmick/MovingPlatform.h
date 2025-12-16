#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECT_14_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorA|Components")
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ActorA|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Moving")
	FVector MovingSpeed = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Moving")
	FRotator RotationSpeed = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Moving")
	float MovedDistance = 0.0f;

	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(Replicated)
	FVector CurrentMovingSpeed;

	void MoveActor(float DeltaTime);
	void RotateActor(float DeltaTime);

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
