#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NumberPlatePuzzleManager.generated.h"

class AGateActor;

UCLASS()
class PROJECT_14_API ANumberPlatePuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	ANumberPlatePuzzleManager();

	virtual void BeginPlay() override;

	void NotifyPlateStepped(int32 PlateNumber);
	void NotifyPlateReleased(int32 PlateNumber);

protected:
	UPROPERTY(Replicated)
	TArray<int32> AnswerNumbers;   

	UPROPERTY()
	TSet<int32> CurrentSteppedPlates;

	UPROPERTY(EditInstanceOnly)
	AGateActor* GateToOpen;

	void CheckClear();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
