#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class PROJECT_14_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorA|Components")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorA|Components")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Moving")
	FVector MovingSpeed = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Moving")
	FRotator RotationSpeed = FRotator(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Moving")
	float MovedDistance = 0.0f;

	FVector StartLocation;

	virtual void BeginPlay() override;
	virtual void Tick(float Deltatime) override;

	void MoveActor(float DeltaTime);
	void RotateActor(float DeltaTime);

	bool ShouldActorReturn() const;
	float GetDistanceMoved() const;

};
