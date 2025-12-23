#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lever.generated.h"

class APlayerCharacter;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class PROJECT_14_API ALever : public AActor
{
	GENERATED_BODY()

public:
	ALever();

	UFUNCTION(Server, Reliable)
	void Server_TryInteract(APlayerCharacter* Player);
protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeverMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* InteractBox;

	UPROPERTY()
	bool bPlayerOverlapping = false;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};
