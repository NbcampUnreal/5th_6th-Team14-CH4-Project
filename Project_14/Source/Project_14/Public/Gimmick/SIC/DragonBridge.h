#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonBridge.generated.h"

UCLASS()
class PROJECT_14_API ADragonBridge : public AActor
{
	GENERATED_BODY()

public:
	ADragonBridge();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, Category = "Movement")
	int32 MaxTargetNum = 8;

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

	// 시작 중복 방지
	UPROPERTY()
	bool bHasStarted = false;

public:
	// 기존: 다리 시작
	UFUNCTION(BlueprintCallable)
	void ToggleState();

private:
	// 실제 시작 로직
	void ToggleState_Internal();

	// 다음 목표 지점
	void NextTarget();

	UFUNCTION()
	void OnRep_TargetNum();
};
