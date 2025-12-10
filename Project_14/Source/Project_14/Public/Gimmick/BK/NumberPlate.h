#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NumberPlate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UWorld;
class AGateActor;    

UCLASS()
class PROJECT_14_API ANumberPlate : public AActor
{
	GENERATED_BODY()

public:
	ANumberPlate();

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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NumberPlate")
	UStaticMeshComponent* PlateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NumberPlate")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NumberPlate")
	int32 PlateNumber = 1;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "NumberPlate")
	AGateActor* GateToOpen;

	bool bCleared = false;
	static int32 GlobalAnswerNumber;
	static UWorld* CachedWorld;
};