#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ButtonActor_A.generated.h"

UCLASS()
class PROJECT_14_API AButtonActor_A : public AActor
{
	GENERATED_BODY()
	
public:	
	AButtonActor_A();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ButtonMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	class AGateActor* TargetGate;

	UPROPERTY(EditAnywhere, Category = "ButtonMode")
	bool bOneTimeActivation = false;

};
