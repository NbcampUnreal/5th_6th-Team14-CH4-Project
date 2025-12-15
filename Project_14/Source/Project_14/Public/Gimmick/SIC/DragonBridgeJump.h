#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonBridgeJump.generated.h"

UCLASS()
class PROJECT_14_API ADragonBridgeJump : public AActor
{
	GENERATED_BODY()

public:
	ADragonBridgeJump();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, Category = "Movement")
	int32 MaxTargetNum = 10;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 200.f;

	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(ReplicatedUsing = OnRep_TargetNum)
	int32 TargetNum = 0;

	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(Replicated)
	bool bIsEnd = false;

	TArray<FVector> Targets;

public:
	UFUNCTION(BlueprintCallable)
	void ToggleState();

private:
	UFUNCTION(Server, Reliable)
	void Server_ToggleState();

	void ToggleState_Internal();
	void NextTarget();

	UFUNCTION()
	void OnRep_TargetNum();
};
