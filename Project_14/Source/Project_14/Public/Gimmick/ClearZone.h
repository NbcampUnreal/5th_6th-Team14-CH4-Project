#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClearZone.generated.h"

class UBoxComponent;
class ACharacter;

UCLASS()
class PROJECT_14_API AClearZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AClearZone();

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

	void CheckClearCondition();

	UFUNCTION()
	void OnGameClear();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ClearBox;

	UPROPERTY()
	TSet<ACharacter*> OverlappingCharacters;

	UPROPERTY(EditAnywhere, Category = "Clear")
	int32 RequiredPlayerCount = 2;

	bool bIsCleared = false;
};