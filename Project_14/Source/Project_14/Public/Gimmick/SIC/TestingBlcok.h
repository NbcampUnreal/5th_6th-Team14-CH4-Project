#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestingBlcok.generated.h"

UCLASS()
class PROJECT_14_API ATestingBlcok : public AActor
{
	GENERATED_BODY()

public:
	ATestingBlcok();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UFUNCTION()
	void OnBlockBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
