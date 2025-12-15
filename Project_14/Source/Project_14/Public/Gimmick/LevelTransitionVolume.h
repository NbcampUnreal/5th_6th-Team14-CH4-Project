#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

class UBoxComponent;
class APlayerState;

UCLASS()
class PROJECT_14_API ALevelTransitionVolume : public AActor
{
	GENERATED_BODY()

public:
	ALevelTransitionVolume();

protected:
	virtual void BeginPlay() override;

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transition")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Transition")
	FName NextLevelName;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Transition")
	int32 RequiredPlayerCount = 2;

private:
	UPROPERTY()
	TSet<TObjectPtr<APlayerState>> OverlappedPlayerStates;

	void TryServerTravel();
};
