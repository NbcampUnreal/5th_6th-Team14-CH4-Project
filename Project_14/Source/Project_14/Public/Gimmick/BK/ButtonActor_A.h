#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ButtonActor_A.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AGateActor;

UCLASS()
class PROJECT_14_API AButtonActor_A : public AActor
{
	GENERATED_BODY()

public:
	AButtonActor_A();

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

	void OpenAllGates();
	void CloseAllGates();

public:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	TArray<AGateActor*> TargetGates;

	
	UPROPERTY(EditAnywhere, Category = "ButtonMode")
	bool bToggleMode = false;

	UPROPERTY(EditAnywhere, Category = "ButtonMode", meta = (EditCondition = "!bToggleMode"))
	bool bOneTimeActivation = false;

	UPROPERTY(VisibleAnywhere, Category = "ButtonMode")
	bool bIsGateOpen = false;
};
